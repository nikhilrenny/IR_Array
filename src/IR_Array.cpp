#include "IR_Array.h"
#include <Arduino.h> 

IR_Array::IR_Array(uint8_t count) : SensorCount(count) { // // Constructor: Initialize an IR_Array with a specified number of sensors

  // All arrays are automatically initialized using vectors
    SensorPins(sensorCount),
    Count(sensorCount, 0),
    Sum(sensorCount, 0.0f),
    Sum_Sq(sensorCount, 0.0f),
    SensorMin(sensorCount, 1023.0f),  // Initialize minimum readings to max possible ADC value (1023)
    SensorMax(sensorCount, 0.0f),
    Range(sensorCount, 0.0f),
    Mean(sensorCount, 0.0f),
    Variance(sensorCount, 0.0f),
    Std_Dev(sensorCount, 0.0f),
    SensorStatus(sensorCount, 0.0f),
    K(2),    // Default Gaussian threshold multiplier
    Sum_Std_Dev(0.0f)
}

void IR_Array::setup(const uint8_t* pins, int k) {
  K = k;
  if (pins != nullptr) {for (uint8_t i = 0; i < SensorCount; i++) {SensorPins[i] = pins[i];}}  // Use provided pin mapping
  else {for (uint8_t i = 0; i < SensorCount; i++){SensorPins[i] = A0 + i;}}  // Default pin mapping: A0, A1, A2, ...
}

void IR_Array::DebugData() {
  Serial.println("------ SENSOR DATA SUMMARY ------");
  Serial.println();

  Serial.println("Index   Count   Min     Max     Range   Mean    StdDev  Status  ");
  Serial.println("----------------------------------------------------------------");

  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(i); Serial.print("\t");
    Serial.print(Count[i]); Serial.print("\t");
    Serial.print(SensorMin[i]); Serial.print("\t");
    Serial.print(SensorMax[i]); Serial.print("\t");
    Serial.print(Range[i]); Serial.print("\t");
    Serial.print(Mean[i]); Serial.print("\t");
    Serial.print(Std_Dev[i]); Serial.print("\t");
    Serial.print(SensorStatus[i]); Serial.println("\t");
  }

  Serial.println("--------------------------------------------------------------- ");
  Serial.print("Sum of Std Devs: "); Serial.println(Sum_Std_Dev);
  Serial.println("-----------------------------------------------");
}

void IR_Array::CalibrateSensors(uint16_t ctime){
  Serial.println("Calibrating IR Sensor Array...");
  unsigned long startTime = millis();
  while (millis() - startTime < ctime){
    for(uint8_t i = 0; i < SensorCount; i++){
      uint16_t SensorValue = analogRead(SensorPins[i]); 
      Sum[i] += SensorValue; 
      Sum_Sq[i] += (float)SensorValue * (float)SensorValue; 
      Count[i]++ ;
      if (SensorValue > SensorMax[i]) {SensorMax[i] = SensorValue;}// record the maximum sensor value
      if (SensorValue < SensorMin[i]) {SensorMin[i] = SensorValue;}// record the minimum sensor value
    }
  }
  for(uint8_t i = 0; i < SensorCount; i++){   // Compute statistics for each sensor
    Mean[i] = Sum[i] / Count[i];
    Variance[i] = (Sum_Sq[i] / Count[i]) - (Mean[i] * Mean[i]);        
    Std_Dev[i] = sqrt(Variance[i]);
    Range[i] = SensorMax[i] - SensorMin[i];
    Sum_Std_Dev += Std_Dev[i];
  }
  
  for(uint8_t i = 0; i < SensorCount; i++){     // Evaluate sensor health using Gaussian distribution (bell curve)
    float tinyValue = Mean[i] - K*Std_Dev[i]; 
    float largeValue = Mean[i] + K*Std_Dev[i];
    float SensorValue = analogRead(SensorPins[i]); 
    if(SensorValue < tinyValue){SensorStatus[i] = 0;}  // Below normal range
    else if(SensorValue > largeValue){SensorStatus[i] = 0.5;}  // Above normal range
    else {SensorStatus[i] = 1;}    // Normal
  }
  Serial.println("Calibration complete!");
}

void IR_Array::Read(uint8_t mode, uint8_t pd ){
  switch(mode){
    case 1: // raw data
      for(uint8_t i = 0; i < SensorCount; i++){     
        float SensorValue = analogRead(SensorPins[i]);
        if(pd == 1){
          char buffer[12];
          dtostrf(SensorValue, 8, 2, buffer);  // width=8, precision=2 dtostrf(float_value, width, precision, buffer)
          Serial.print(buffer);
          Serial.print(" | ");
        }
        else { return; }
      }
      if(pd == 1){Serial.println();} else { return; }
      break;

    case 2: // z-score filtered readings
      for(uint8_t i = 0; i < SensorCount; i++){     
        float SensorValue = analogRead(SensorPins[i]);
        float zScore = (SensorValue - Mean[i])/Std_Dev[i];
        if(pd == 1){
          char buffer[12];
          dtostrf(zScore, 8, 2, buffer);  // width=8, precision=2 dtostrf(float_value, width, precision, buffer)
          Serial.print(buffer);
          Serial.print(" | ");
        }
        else { return; }
      }
      if(pd == 1){Serial.println();} else { return; }
      break;
    
    case 3:  //  thresholded normalized readings
      for(uint8_t i = 0; i < SensorCount; i++){     // Dynamic Normalization
        float SensorValue = analogRead(SensorPins[i]);
        float zScore = (SensorValue - Mean[i])/Std_Dev[i];
        if(zScore < 0){ zScore = 0;}
        else {zScore = 1;}
        if(pd == 1){
          char buffer[12];
          dtostrf(zScore, 8, 2, buffer);  // width=8, precision=2 dtostrf(float_value, width, precision, buffer)
          Serial.print(buffer);
          Serial.print(" | ");
        }
        else { return; }
      }
      if(pd == 1){Serial.println();} else { return; }
      break;
  
    default:    // Position left/center/right
      float Center = 0;
      uint8_t ActiveCount = 0;
      char* POS; // Move Position
      for(uint8_t i = 0; i < SensorCount; i++){     // pos
        float SensorValue = analogRead(SensorPins[i]);
        float zScore = (SensorValue - Mean[i])/Std_Dev[i];
        if(zScore < 0){ zScore = 0;}
        else {zScore = 1;}
        if(zScore == 1){Center += i; ActiveCount++;} // average index
      }
      if (ActiveCount > 0) Center /= ActiveCount;
      if (Center < 4.00 && Center > 3.00){ POS = "Center";}
      else if ( Center > 4.00){ POS = "Right";}
      else if ( Center < 3.00){ POS = "Left";}
      if(pd == 1){
        Serial.print(Center);
        Serial.print(" | ");
        Serial.println(POS);
      }
      else { return; }
      break;  
  }
}


