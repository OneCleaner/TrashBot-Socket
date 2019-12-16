#include <ComponentObject.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <RangeSensor.h>
#include <SparkFun_VL53L1X.h>
#include <vl53l1x_class.h>
#include <vl53l1_error_codes.h>

#include "Wire.h"
#define BAUD 115200
#define GPS_BAUD 9600

extern "C" {
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCA_ADDR 0x70
#define VL53L1X_ADDR 0x29
#define GPSPin 12

TinyGPSPlus gps;
SoftwareSerial ss(GPSPin, -1);

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

bool debug = true;

int distance[8] = {0};

String coordinates = "";
String date = "";
String time = "";

unsigned velocity = 0;

void setup(void){

  Serial.begin(BAUD);
  while(!Serial);

  ss.begin(GPS_BAUD);
  delay(100);

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

  if(debug){ Serial.print("\nTCA ADDR: 0x"); Serial.println(TCA_ADDR, HEX); }
  delay(1000);
  for (uint8_t i = 0; i < 8; i++) {
    tcaselect(i);
    if(debug) { Serial.print("TCA Port #"); Serial.println(i); }
    if (distanceSensor.begin() == 0) //Begin returns 0 on a good init
      if(debug) Serial.println("  Sensor online!");
    delay(333);
  }
}

void loop(void){

  String data = "";
  for (uint8_t i = 0; i < 8; i++) {
    tcaselect(i);
    distanceSensor.startRanging();
      distance[i] = distanceSensor.getDistance();
    distanceSensor.stopRanging();
    if ((i == 2 || i == 3 || i == 7) && debug) {
      Serial.print("Distanza S");
      Serial.print(i);
      Serial.print(" -> ");
      Serial.print(distance[i]);
      Serial.println(" mm\t");
      delay(1000);
    } 
     
  } 

  if(distance[2] < 500 || distance[3] < 500 || distance[7] < 1500) {
    setVelocity(0);
  } else if(isMoving) setVelocity(255);

  while(ss.available() > 0){
    if(gps.encode(ss.read())){
      coordinates = parseGPS("coordinates");
      date = parseGPS("date");
      time = parseGPS("time");
    }

    if(millis() > 50000 && gps.charsProcessed() < 10){
      Serial.println("GPS non rilevato.");
      while(true);
    }
  }

  while(Serial.available() > 0){
    data = Serial.readString();


    if(data == "debug-on"){
      debug = true;
    }
    else if(data = "debug-off"){
      debug = false;
    }

    else if(data = "orario"){
      Serial.println(time);
    }

    else if(data = "data"){
      Serial.println(date);
    }

    else if(data = "posizione"){
      Serial.println(coordinates);
    }
    
    else if(data == "dritto"){
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
  if (i > 8) return;

  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << i);
  Wire.endTransmission();

  return;
}

String parseGPS(String type){
  String str = "";
  if(type = "coordinates"){
    if(gps.location.isValid()){
      str = (String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6));
    } else str = "InvalidCoordinates";
  }
  else if(type = "date"){
    if(gps.date.isValid()){
      str = (String(gps.date.day(), 2) + "/" + String(gps.date.month(), 2) + "/" + String(gps.date.year(), 2));
    } else str = "invalidDate";
  }
  else if(type = "time"){
    if(gps.time.isValid()){
      str = (String(gps.time.hour(), 2) + ":" + String(gps.time.minute(), 2) + ":" + String(gps.time.second(), 2));
    } else str = "invalidTime";
  }
  return str;
}
