// uncomment if you want to run this on an ESP32
// not needed if you're using an ESP8266
// #include <EmbAJAXOutputDriverESPAsync.h>

#include <EEPROM.h>
#include <EmbAJAX.h>
#include <Wire.h>

#include "SensorAdjustmentPage.h"
#include "i2cSimpleTransfer.h"

// constants for storing config values
#define CONFIG_VERSION "v0"
#define CONFIG_START 32

// #################################################
// Wifi and webserver configuration
// #################################################

#ifdef __5_PANEL__
  #define WIFI_SSID "Pump It Up Pad"
#else
  #define WIFI_SSID "Dance Dance Revolution Pad"
#endif

#define WIFI_PASSWORD "danceboiz2k19"

// setup web server and register it with EmbAJAX
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

// buffers to hold the strings to display on the page for our poll
// rates and sensor values
char p1pollBuf[8], p2pollBuf[8];
char p1a1buf[8], p1a2buf[8], p1a3buf[8], p1a4buf[8], p1a5buf[8], p2a1buf[8], p2a2buf[8], p2a3buf[8], p2a4buf[8], p2a5buf[8];
char* pollbufs[2] = { p1pollBuf, p2pollBuf };
char* displayBufs[2][5] = { { p1a1buf, p1a2buf, p1a3buf, p1a4buf, p1a5buf },
                          { p2a1buf, p2a2buf, p2a3buf, p2a4buf, p2a5buf } };
EmbAJAXMutableSpan* pollrateDisplays[2] = { &p1pollrate, &p2pollrate };
EmbAJAXMutableSpan* valueDisplays[2][5] = { { &p1a1display, &p1a2display, &p1a3display, &p1a4display, &p1a5display },
                                         { &p2a1display, &p2a2display, &p2a3display, &p2a4display, &p2a5display } };
EmbAJAXSlider* thresholdSliders[2][5] = { { &p1a1slider, &p1a2slider, &p1a3slider, &p1a4slider, &p1a5slider },
                                       { &p2a1slider, &p2a2slider, &p2a3slider, &p2a4slider, &p2a5slider } };

// ################################################
// Sensor and I2C configuration 
// ################################################

#define I2C_SLAVE_1 0x0A
#define I2C_SLAVE_2 0x0B

int i2cAddrs[2] = { I2C_SLAVE_1, I2C_SLAVE_2 };

#ifdef __5_PANEL__
  #define NUM_INPUTS 5
#else
  #define NUM_INPUTS 4
#endif

// the current thresholds for each arrow
struct THRESHOLD_DATA {
  uint16_t thresholds[5];
};

// the current values of each sensor, and the last poll rate
struct PAD_READOUT_DATA {
  uint16_t pressures[5];
  uint16_t pollRate;
};

THRESHOLD_DATA thresholdData[2];
PAD_READOUT_DATA padReadoutData[2];

// allow us to store threshold values in EEPROM
struct ConfigStore {
  char version[4];
  int thresholds[2][5];
} storage = {
  // defaults
  CONFIG_VERSION,
  { { 800, 800, 800, 800, 800 },
    { 800, 800, 800, 800, 800 } }
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
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  
  // start I2C communications
  Wire.begin();
  Wire.setClock(400000);
  
  // load the saved threshold values so we can send them to the arduinos
  loadConfig();

  // setup a wifi network to connect to this pad ad-hoc
  Serial.begin(115200);
  boolean result = WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  
  if (result) {
    Serial.println("Wifi ready!");
    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);
    Serial.print("Password: ");
    Serial.println(WIFI_PASSWORD);
  } else {
    Serial.println("Failed to setup the wifi!");
  }

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
    for (int i = 0; i < 2; i++) {
      Wire.beginTransmission(i2cAddrs[i]);
      i2cSimpleWrite(thresholdData[i]);
      Wire.endTransmission();
  
      // receive the current readings from the slaves
      Wire.requestFrom(i2cAddrs[i], sizeof(padReadoutData[i]));
      delay(10);
      
      if (Wire.available() == sizeof(padReadoutData[i])) {
          Serial.println("Receiving I2C pad readout");
          i2cSimpleRead(padReadoutData[i]);
  
          for (int j = 0; j < NUM_INPUTS; j++) {
            (valueDisplays[i][j])->setValue(itoa(padReadoutData[i].pressures[j], displayBufs[i][j], 10));
          }
          
          pollrateDisplays[i]->setValue(itoa(padReadoutData[i].pollRate, pollbufs[i], 10));
      }
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
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      (thresholdSliders[i][j])->setValue(thresholdData[i].thresholds[j]);
    }
  }
}

/**
   Handle updates coming from the client, to change sensitivity thresholds.
*/
void handleUpdates() {
  bool shouldSave = false;

  for (int i = 0; i < 2; i ++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      int newValue = (thresholdSliders[i][j])->intValue();
  
      // if the incoming value for this arrow is not what we had saved, then store
      // the value and update EEPROM
      if (thresholdData[i].thresholds[j] != newValue) {
        thresholdData[i].thresholds[j] = newValue;
        shouldSave = true;
      }
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
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      thresholdData[i].thresholds[j] = storage.thresholds[i][j];
    }
  }
}

/**
   Save the config values to EEPROM.
*/
void saveConfig() {
  // update the data we're going to store
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < NUM_INPUTS; j++) {
      storage.thresholds[i][j] = thresholdData[i].thresholds[j];
    }
  }

  // write the data
  for (unsigned int t = 0; t < sizeof(storage); t++) {
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
  }

  EEPROM.commit();
}
