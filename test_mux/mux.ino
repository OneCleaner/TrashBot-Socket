#include <ComponentObject.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

#include "Wire.h"
#define BAUD 115200

extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR 0x70
#define ADDR 0x29

SFEVL53L1X distanceSensor(Wire, -1, -1);

void tcaselect(uint8_t i) {
  if (i > 7) return;
  
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

 
void setup(void) {
  
    Serial.begin(BAUD);
    while(!Serial);
  
    pinMode(LED_BUILTIN, OUTPUT);
    delay(1000);
 
    Wire.begin();
    Serial.println("\nTCAScanner ready!");
    delay(1000);
    
    for (uint8_t i = 0; i < 7; i++) {
      tcaselect(i);
      //distanceSensor.setI2CAddress(ADDR);
      Serial.print("TCA Port #"); Serial.println(i);
      if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
        Serial.println("Sensor online!");
      else Serial.println("not found");
        delay(333);
    }

}

int distance[7] = {0};

void loop(void) {
  for (uint8_t i = 0; i < 7; i++) {
    tcaselect(i);
    Serial.print("TCA Port #"); Serial.println(i);
    distanceSensor.startRanging();
      distance[i] = distanceSensor.getDistance();
    distanceSensor.stopRanging();

    Serial.print("  Distance (mm): "); Serial.println(distance[i]);
  }
  if(distance[0] < 250 || distance[1] < 250){
    digitalWrite(LED_BUILTIN, HIGH);
  } else digitalWrite(LED_BUILTIN, LOW);
  delay(0);
}
