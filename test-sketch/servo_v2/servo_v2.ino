#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
int switchState = 0;

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
  Serial.println("Servo Ready");
  Serial.println("Reset servo position");
  Serial.println("Servo is at initial position");

  pinMode(12, INPUT);
}

void loop() {
  switchState = digitalRead(12);
  if (switchState == HIGH) {
    Serial.println("Initial position: 0");
    for (pos = 0; pos <= 25; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);                       // waits 15ms for the servo to reach the position
    }
    //delay(3000);
    Serial.println("Final position: 90");
    for (pos = 25; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(5);                       // waits 15ms for the servo to reach the position
    }
  }
}
