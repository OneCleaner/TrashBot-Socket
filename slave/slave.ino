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

#define TCA_ADDR 0x70
#define VL53L1X_ADDR 0x29

SFEVL53L1X distanceSensor(Wire, -1, -1);

#define pwm_a 6  //PWM control 
#define pwm_b 9 
#define pwm_c 5
#define pwm_d 3

bool isMoving = false;

#define led_freni A0
#define led_fari 4

#define dir_a 7  //Motore posteriore destro
#define dir_b 8  //Motore anteriore destro
#define dir_c 11 //Motore posteriore sinistro
#define dir_d 10 //Motore anteriore sinistro


int distance[7] = {0};

unsigned velocity = 0;

void setup(void){

  Serial.begin(BAUD);
  while(!Serial);
  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(pwm_a, OUTPUT);  //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(pwm_c, OUTPUT);
  pinMode(pwm_d, OUTPUT);
  
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  pinMode(dir_c, OUTPUT);
  pinMode(dir_d, OUTPUT);

  pinMode(led_freni, OUTPUT);
  pinMode(led_fari, OUTPUT);

  //mettiamo così che parta in avanti alla prima
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  digitalWrite(dir_c, HIGH); 
  digitalWrite(dir_d, HIGH);
  digitalWrite(led_fari, HIGH);
  digitalWrite(led_freni, HIGH);
  
  setVelocity(velocity);

  Wire.begin();
  delay(1000);
  
  Serial.print("\nTCA ADDR: 0x"); Serial.println(TCA_ADDR, HEX);
  delay(1000);
  for (uint8_t i = 0; i < 7; i++) {
    tcaselect(i);
    Serial.print("TCA Port #"); Serial.println(i);
    if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
      Serial.println("  Sensor online!");
    delay(333);
  }
}

void loop(void){
  
  String data = "";
  for (uint8_t i = 0; i < 7; i++) {
    tcaselect(i);
    distanceSensor.startRanging();
      distance[i] = distanceSensor.getDistance();
    distanceSensor.stopRanging();
  }
  
  if(distance[2] < 80 || distance[3] < 80) {
    setVelocity(0);
  } else if(isMoving) setVelocity(255);

  while(Serial.available() > 0){
    data = Serial.readString();
  
    if(data == "dritto"){
        digitalWrite(dir_a, HIGH);
        digitalWrite(dir_b, HIGH);
        digitalWrite(dir_c, HIGH); 
        digitalWrite(dir_d, HIGH);
        digitalWrite(led_freni, LOW);
        setVelocity(velocity);
        Serial.println("Vado dritto");
    }
    
    else if(data == "destra"){
        digitalWrite(dir_a, LOW);   //ruote di destra
        digitalWrite(dir_b, LOW);
        digitalWrite(dir_c, HIGH); 
        digitalWrite(dir_d, HIGH);
        digitalWrite(led_freni, LOW);
        setVelocity(velocity);
        Serial.println("Svolto a destra");

    }

    else if(data == "gira-destra"){
        analogWrite(pwm_a, velocity -40);  
        analogWrite(pwm_b, velocity -40);
        digitalWrite(dir_a, HIGH);   //ruote di destra
        digitalWrite(dir_b, HIGH);
        digitalWrite(dir_c, HIGH); 
        digitalWrite(dir_d, HIGH);
        digitalWrite(led_freni, LOW);
        setVelocity(velocity);
        Serial.println("Giro a destra");
    }
    
    else if(data == "sinistra"){
        digitalWrite(dir_a, HIGH);
        digitalWrite(dir_b, HIGH);
        digitalWrite(dir_c, LOW); //ruote di sinistra  
        digitalWrite(dir_d, LOW);
        digitalWrite(led_freni, LOW); 
        setVelocity(velocity);
        Serial.println("Svolto a sinistra");
    }

    else if(data == "gira-sinistra"){
        analogWrite(pwm_c, velocity -40);  
        analogWrite(pwm_d, velocity -40);
        digitalWrite(dir_a, HIGH);   //ruote di destra
        digitalWrite(dir_b, HIGH);
        digitalWrite(dir_c, LOW); 
        digitalWrite(dir_d, LOW);
        digitalWrite(led_freni, LOW);
        setVelocity(velocity);
        Serial.println("giro a sinistra");
    }
    else if(data == "retro"){
        digitalWrite(dir_a, LOW);
        digitalWrite(dir_b, LOW);
        digitalWrite(dir_c, LOW); //ruote di sinistra  
        digitalWrite(dir_d, LOW);
        digitalWrite(led_freni, HIGH);
        setVelocity(velocity);
        Serial.println("Torno indietro");
    }
    
    else if(data == "spegni"){
        digitalWrite(led_freni, HIGH);
        setVelocity(0);
        isMoving = false;
        Serial.println("Spengo i motori");
    }
    
    else if(data == "accendi"){
        digitalWrite(led_freni, LOW);
        setVelocity(255);
        isMoving = true;
        Serial.println("Accendo i motori");
    } 

    else if(isNum(data) && data.toInt() > 0 && data.toInt() <= 255){
        setVelocity(data.toInt());
        Serial.println("cambiata velocità a: " + data);
    }
    
    else {
      Serial.println("Comando non riconosciuto: " + data);
    }
  }
}


//inserire solo velocità tra 0 e 255, altrimenti la funzione non cambierà lo stato del motore
void setVelocity(unsigned v){

  velocity = v;
  
  if(v > 255)
    return;

  analogWrite(pwm_a, v);  
  analogWrite(pwm_b, v);  //minimo 190 o non partono wewo
  analogWrite(pwm_c, v);
  analogWrite(pwm_d, v);

  return;
}

bool isNum(String str){

  for(int i = 0; i < str.length(); i++){
    if(!isDigit(str[i]))
      return false;
  }

  return true;
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
  
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  

  return;
}
