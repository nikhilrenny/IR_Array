#include <IR_Array.h>

IR_Array ir(4);  // 4 sensors

void setup() {
  Serial.begin(115200);
  while(!Serial);

  ir.setup();               // default pins
  ir.CalibrateSensors(5000); // calibrate for 5 seconds
}

void loop() {
  // Read raw values from analog pins without printing
  for (uint8_t i = 0; i < 4; i++) {
    float rawValue = analogRead(ir.getSensorPins()[i]);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(rawValue);
  }

  // Access statistics from IR_Array object
  const float* means = ir.getMean();
  const float* stddevs = ir.getStdDev();
  const float* minVals = ir.getSensorMin();
  const float* maxVals = ir.getSensorMax();

  Serial.println("Statistics:");
  for (uint8_t i = 0; i < 4; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" -> Mean: ");
    Serial.print(means[i]);
    Serial.print(", StdDev: ");
    Serial.print(stddevs[i]);
    Serial.print(", Min: ");
    Serial.print(minVals[i]);
    Serial.print(", Max: ");
    Serial.println(maxVals[i]);
  }

  delay(1000); // wait 1 second before next read
}