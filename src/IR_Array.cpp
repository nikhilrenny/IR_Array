#include "IR_Array.h"

#include "IR_Array.h"

IR_Array::IR_Array(uint8_t count) : SensorCount(count) {

  SensorPins   = new uint8_t[count];
  Count        = new uint16_t[count]();
  Sum          = new float[count]();
  Sum_Sq       = new float[count]();
  SensorMin    = new float[count]();
  SensorMax    = new float[count]();
  Range        = new float[count]();
  Mean         = new float[count]();
  Variance     = new float[count]();
  Std_Dev      = new float[count]();
  SensorStatus = new float[count]();
  for (uint8_t i = 0; i < count; i++) {SensorMin[i] = 1023;}
  K = 2;
  Sum_Std_Dev = 0;
}

void IR_Array::setup(const uint8_t* pins, int k) {
  K = k;
  if (pins != nullptr) {for (uint8_t i = 0; i < SensorCount; i++) {SensorPins[i] = pins[i];}}
  else {for (uint8_t i = 0; i < SensorCount; i++){SensorPins[i] = A0 + i;}}
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
  for(uint8_t i = 0; i < SensorCount; i++){   // Statistics
    Mean[i] = Sum[i] / Count[i];
    Variance[i] = (Sum_Sq[i] / Count[i]) - (Mean[i] * Mean[i]);        
    Std_Dev[i] = sqrt(Variance[i]);
    Range[i] = SensorMax[i] - SensorMin[i];
    Sum_Std_Dev += Std_Dev[i];
  }
  
  for(uint8_t i = 0; i < SensorCount; i++){     // Sensor health - Gaussian distribution - Bell Curve
    float tinyValue = Mean[i] - K*Std_Dev[i]; 
    float largeValue = Mean[i] + K*Std_Dev[i];
    float SensorValue = analogRead(SensorPins[i]); 
    if(SensorValue < tinyValue){SensorStatus[i] = 0;}
    else if(SensorValue > largeValue){SensorStatus[i] = 0.5;}
    else {SensorStatus[i] = 1;}
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

    case 2: // filteredata
      for(uint8_t i = 0; i < SensorCount; i++){     
        float SensorValue = analogRead(SensorPins[i]);
        float NormalizedValue = (SensorValue - Mean[i])/Std_Dev[i];
        if(pd == 1){
          char buffer[12];
          dtostrf(NormalizedValue, 8, 2, buffer);  // width=8, precision=2 dtostrf(float_value, width, precision, buffer)
          Serial.print(buffer);
          Serial.print(" | ");
        }
        else { return; }
      }
      if(pd == 1){Serial.println();} else { return; }
      break;
    
    case 3: // normalized data
      for(uint8_t i = 0; i < SensorCount; i++){     // Dynamic Normalization
        float SensorValue = analogRead(SensorPins[i]);
        float DyanmicNormalizedValue = (SensorValue - Mean[i])/Std_Dev[i];
        if(DyanmicNormalizedValue < 0){ DyanmicNormalizedValue = 0;}
        else {DyanmicNormalizedValue = 1;}
        if(pd == 1){
          char buffer[12];
          dtostrf(DyanmicNormalizedValue, 8, 2, buffer);  // width=8, precision=2 dtostrf(float_value, width, precision, buffer)
          Serial.print(buffer);
          Serial.print(" | ");
        }
        else { return; }
      }
      if(pd == 1){Serial.println();} else { return; }
      break;
  
    default:
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


