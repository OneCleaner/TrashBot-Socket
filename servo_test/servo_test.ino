#include <Servo.h>
#include <String.h>

#define BAUD 9600
Servo servo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

String data = "0";

void setup() {

  Serial.begin(BAUD);
  while(!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  servo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
    if(Serial.available() > 0)
      data = Serial.readString();

    servo.write(data.toInt());
    //servo.write(0);
}
