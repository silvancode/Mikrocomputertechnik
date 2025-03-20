#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "dispatcher.hpp"

// Set up an instance of the Adafruit TCS34725 library
// The parameters are (integrationTime, gain). 
// Refer to the library examples or documentation for choosing appropriate values.
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
    TCS34725_INTEGRATIONTIME_2_4MS,   // 2.4ms integration time
    TCS34725_GAIN_1X                 // 1x gain
);

// 8 digital pins for columns:
const int cols[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// 3 open-collector pins for rows:
const int rows[3] = {22, 23, 24};

// The LED matrix "on/off" states (3 rows × 8 columns)
int ledmatrix[3][8];

// Example values: 255 => full bar, 128 => half bar, 64 => quarter bar
int CalcVal[3];

void UpdateColorSensor() {
  // Variables to hold raw sensor readings
  uint16_t rRaw, gRaw, bRaw, cRaw;
  // Read the sensor
  tcs.getRawData(&rRaw, &gRaw, &bRaw, &cRaw);
  // Avoid division by zero
  if (cRaw == 0) {
    Serial.println("cRaw == 0, skipping...");
    delay(500);
    return;
  }

  // Convert raw values to more standard 0-255 range
  // You can adjust the scaling factor to fit your use case
  float rFloat = (float)rRaw / (float)cRaw * 255.0;
  float gFloat = (float)gRaw / (float)cRaw * 255.0;
  float bFloat = (float)bRaw / (float)cRaw * 255.0;

  // Convert floats to integers
  int rVal = (int)rFloat;
  int gVal = (int)gFloat;
  int bVal = (int)bFloat;

  // Print out the 3 color values
  Serial.print("R: ");
  Serial.print(rVal);
  Serial.print("  G: ");
  Serial.print(gVal);
  Serial.print("  B: ");
  Serial.println(bVal);

  CalcVal[0] = rVal;
  CalcVal[1] = gVal;
  CalcVal[2] = bVal;
}

void UpdateLedMatrix() {
  //---------------------------------
  // Update ledmatrix[][] from CalcVal
  //---------------------------------
  for(int row = 0; row < 3; row++){
    // Map 0..255 -> 0..8 columns lit
    int columnsLit = map(CalcVal[row], 0, 255, 0, 2047);
    for(int col = 0; col < 8; col++){
      // Set ledmatrix[row][col] to the specific 0..255 analog value for every LED
      if (columnsLit > 255) {
        ledmatrix[row][col] = 255;
        columnsLit -= 255;
      }else{
        ledmatrix[row][col] = columnsLit;
        columnsLit = 0;
      }
    }
  }

  //---------------------------------
  // Multiplex the matrix
  //---------------------------------
  for(int r = 0; r < 3; r++){
    // Turn OFF all rows (active LOW => write HIGH)
    for(int rr = 0; rr < 3; rr++){
      digitalWrite(rows[rr], HIGH);
    }

    // Set columns for the current row
    for(int c = 0; c < 8; c++){
      analogWrite(cols[c], ledmatrix[r][c]);
    }

    // Enable just this row (active LOW => write LOW)
    digitalWrite(rows[r], LOW);

    // Wait a little so we can see this row (2-5 ms typical)
    delay(5);
  }

  digitalWrite(rows[2], HIGH);
}

Dispatcher disp;
Task sensorTask(UpdateColorSensor, 50);
Task ledTask(UpdateLedMatrix, 20);

void setup() {
  // Columns: set as outputs
  for(int c = 0; c < 8; c++){
    pinMode(cols[c], OUTPUT);
    // Default off by writing LOW
    analogWrite(cols[c], 0);
  }

  // Rows: set as outputs
  for(int r = 0; r < 3; r++){
    pinMode(rows[r], OUTPUT);
    // Default off by writing HIGH
    digitalWrite(rows[r], HIGH);
  }

  Serial.begin(9600);
  //Serial.println("\n");
  Serial.println("TCS34725 RGB sensor test");

  // Initialize the sensor
  if (tcs.begin()) {
    Serial.println("Found TCS34725 sensor!");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  disp.AddTask(sensorTask);
  disp.AddTask(ledTask);
}

void loop() {
  disp.Tick();
}
