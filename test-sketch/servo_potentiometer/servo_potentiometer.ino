
// Import library 
#include <Servo.h>


// variables and constants

int const potPin = A0;
int potVal;
int angle;


// Instatiate servo object
Servo myServo;


void setup() {
  // put your setup code here, to run once:
  myServo.attach(9);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  potVal = analogRead(potPin);
  Serial.print("potVal: ");
  Serial.print(potVal);

  
  // servo moves from 0, 180 and potentiometer reading will vary from 0, 1023. 
  angle = map(potVal, 0, 1023, 0, 179);
  Serial.print(", angle: ");
  Serial.println(angle);
  myServo.write(angle);
  delay(15);
}
