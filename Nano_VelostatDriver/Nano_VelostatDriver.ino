#include "i2cSimpleTransfer.h"

// uncomment this for 5-panel (PIU) mode
// #define __5_PANEL__ 
// uncomment this for player 2 mode
// #define __PLAYER_2__ 

// these are the default sensitivity values we'll use, until
// overridden by the controller board
#define DEFAULT_SENSITIVITY 800

// this board's I2C address
#ifdef __PLAYER_2__
  #define I2C_ADDR 0x0B
#else
  #define I2C_ADDR 0x0A
#endif
  

// keep track of the current sensor values and what the thresholds
// are before we trigger a button press
#ifdef __5_PANEL__
  #define NUM_INPUTS 5
#else
  #define NUM_INPUTS 4
#endif

int inputs[] = { A0, A1, A2, A3, A6 };
int outputs[] = { 2, 3, 4, 5, 6 };

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

// keep track of poll rate
long lastMillis = 0;
int loops = 0;

/**
 * Setup the IO pins
 */
void setup() {
  Serial.begin(9600);

  // init IO pins
  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(inputs[i], INPUT_PULLUP);
    pinMode(inputs[i], HIGH);
    pinMode(outputs[i], OUTPUT);
    thresholdData.thresholds[i] = DEFAULT_SENSITIVITY;
  }

  // init I2C
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  Wire.begin(I2C_ADDR);
  Wire.setClock(400000);
  Wire.onRequest(requestEvent);
  Wire.onReceive (receiveEvent);
}

/**
 * Main program loop, drives the sensors and
 * updates the ESP32 SPI master
 */
void loop() {
  // keep track of our poll rate
  long currentMillis = millis();
  long deltaMillis = currentMillis - lastMillis;
  loops++;

  // every second, update the poll rate
  if (deltaMillis > 1000) {
    padReadoutData.pollRate = (loops / (deltaMillis / 1000.0f));

    Serial.print("Current poll rate: ");
    Serial.print(padReadoutData.pollRate);
    Serial.println(" Hz");

    lastMillis = currentMillis;
    loops = 0;
  }

  // check if any of the buttons are pressed
  for (int i = 0; i < NUM_INPUTS; i++) {
    padReadoutData.pressures[i] = analogRead(inputs[i]);

    if (padReadoutData.pressures[i] <= thresholdData.thresholds[i]) {
      digitalWrite(outputs[i], LOW);
    } else {
      digitalWrite(outputs[i], HIGH);
    }
  }
}

/**
 * Send the pad readouts and poll rate to the I2C master.
 */
void requestEvent() {
    Serial.println("Sending pad readouts to I2C master");
    i2cSimpleWrite(padReadoutData);
}

/**
 * Receive new threshold values from the I2C master.
 */
void receiveEvent (int len) {
    Serial.println("Reading threshold data from I2C master");
    
    if (len == sizeof(thresholdData)){
        i2cSimpleRead(thresholdData);
        Serial.print("New thresholds: ");
        
        for (int i = 0; i < NUM_INPUTS; i++) {
          Serial.print(thresholdData.thresholds[i]);
          Serial.print(" ");
        }
        
        Serial.println();
    }
 }
