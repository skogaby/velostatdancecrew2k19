#include <AsyncTCP.h>
#include <EEPROM.h>
#include <EmbAJAXOutputDriverESPAsync.h>
#include <EmbAJAX.h>
#include <SPI.h>
#include <Wire.h>

#include "SensorAdjustmentPage.h"
#include "i2cSimpleTransfer.h"

// constants for storing config values
#define CONFIG_VERSION "v0"
#define CONFIG_START 32

// #################################################
// Wifi and webserver configuration
// #################################################

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

// setup web server and register it with EmbAJAX
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

// buffers to hold the strings to display on the page for our poll
// rate and sensor values
char pollBuf[8];
char p1a1buf[8], p1a2buf[8], p1a3buf[8], p1a4buf[8], p1a5buf[8];
char* displayBufs[] =  { p1a1buf, p1a2buf, p1a3buf, p1a4buf, p1a5buf };
EmbAJAXMutableSpan* valueDisplays[] = { &p1a1display, &p1a2display, &p1a3display, &p1a4display, &p1a5display };
EmbAJAXSlider* thresholdSliders[] = { &p1a1slider, &p1a2slider, &p1a3slider, &p1a4slider, &p1a5slider };

// ################################################
// Sensor and I2C configuration 
// ################################################

#define I2C_SLAVE_1 0x0A
#define I2C_SLAVE_2 0x0B

int numInputs = 5;

// the current thresholds for each arrow
struct THRESHOLD_DATA {
  uint16_t thresholds[5];
};

// the current values of each sensor, and the last poll rate
struct PAD_READOUT_DATA {
  uint16_t pressures[5];
  uint16_t pollRate;
};

THRESHOLD_DATA thresholdData;
PAD_READOUT_DATA padReadoutData;

// allow us to store threshold values in EEPROM
struct ConfigStore {
  char version[4];
  int thresholds[5];
} storage = {
  // defaults
  CONFIG_VERSION,
  { 800, 800, 800, 800, 800 }
};

// timer so we can update the attached Arduinos
long lastMillis = 0;

// function prototypes
void initPage();
void handleUpdates();
void loadConfig();
void saveConfig();

/**
   Start the web server and setup comms with the slaves
*/
void setup() {
  // start I2C communications
  Wire.begin();
  Wire.setClock(400000);
  
  // load the saved threshold values so we can send them to the arduinos
  loadConfig();

  // connect to wifi network with SSID and password
  Serial.begin(115200);
  Serial.print("Connecting to wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // setup the EmbAJAX content handlers and start the webserver
  driver.installPage(&page, "/", handleUpdates);
  server.begin();

  initPage();
}

/**
   Main program loop, keeps track of poll rate and updates the UI elements
   with current sensor values.
*/
void loop() {
  // keep track of our poll rate
  long currentMillis = millis();

  // every second, we need to update the poll rate display,
  // and send/receive updates from the SPI slaves to adjust
  // sensitivities and get the latest sensor readings
  if (currentMillis - lastMillis > 1000) {
    Serial.println("Updating the Arduino I2C slaves");

    // send the slaves the current thresholds
    Wire.beginTransmission(I2C_SLAVE_1);
    i2cSimpleWrite(thresholdData);
    Wire.endTransmission();

    // receive the current readings from the slaves
    Wire.requestFrom(I2C_SLAVE_1, sizeof(padReadoutData));
    
    if (Wire.available() == sizeof(padReadoutData)) {
        Serial.println("Data available from I2C slave");
        i2cSimpleRead(padReadoutData);

        for (int i = 0; i < numInputs; i++) {
          Serial.print(padReadoutData.pressures[i]);
          Serial.print(" ");
          (valueDisplays[i])->setValue(itoa(padReadoutData.pressures[i], displayBufs[i], 10));
        }

        Serial.println();

        pollrate.setValue(itoa(padReadoutData.pollRate, pollBuf, 10));
    }

    lastMillis = currentMillis;
  }

  // handle web clients
  driver.loopHook();
}

/**
   Set the initial values for the page controls.
*/
void initPage() {
  for (int i = 0; i < numInputs; i++) {
    (thresholdSliders[i])->setValue(thresholdData.thresholds[i]);
  }
}

/**
   Handle updates coming from the client, to change sensitivity thresholds.
*/
void handleUpdates() {
  bool shouldSave = false;

  for (int i = 0; i < numInputs; i++) {
    int newValue = (thresholdSliders[i])->intValue();

    // if the incoming value for this arrow is not what we had saved, then store
    // the value and update EEPROM
    if (thresholdData.thresholds[i] != newValue) {
      thresholdData.thresholds[i] = newValue;
      shouldSave = true;
    }
  }

  if (shouldSave) {
    saveConfig();
  }
}

/**
   Loads the config values from EEPROM.
*/
void loadConfig() {
  EEPROM.begin(512);

  // If nothing is found it will use the default settings.
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]) {
    for (unsigned int t = 0; t < sizeof(storage); t++) {
      *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
    }
  }

  // update the thresholds with what we read from EEPROM
  for (int i = 0; i < numInputs; i++) {
    thresholdData.thresholds[i] = storage.thresholds[i];
  }
}

/**
   Save the config values to EEPROM.
*/
void saveConfig() {
  // update the data we're going to store
  for (int i = 0; i < numInputs; i++) {
    storage.thresholds[i] = thresholdData.thresholds[i];
  }

  // write the data
  for (unsigned int t = 0; t < sizeof(storage); t++) {
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
  }

  EEPROM.commit();
}
