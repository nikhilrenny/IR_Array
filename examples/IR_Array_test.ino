#include <IR_Array.h>
#include <Arduino.h>


// Create a sensor array with 8 sensors
IR_Array ir(8);

// Optional: custom sensor pin list
const uint8_t Pins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};

void setup() {
  Serial.begin(921600);
  delay(2000);

  ir.setup(Pins);      // pins array, K = 2
  ir.CalibrateSensors();
  ir.DebugData();         // print stats table
}

void loop() {
  ir.Read(0,1);       // Mode 1 = Raw, Mode 2 = Filtered, Mode 3 = Normalized, Mode 0 = direction
  delay(500);         // refresh
}
