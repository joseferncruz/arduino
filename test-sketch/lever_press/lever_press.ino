
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <millisDelay.h>

// tutorials:
// https://www.instructables.com/id/Coding-Timers-and-Delays-in-Arduino/

// Lever pressing with random reward at random n lever presses (average 1 reward per 



AlarmId id;

int lever_state = 0;


void deliver_reward(){
  Serial.println("Deliver reward");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  Serial.println("Lever Press Testing");

  

  // Set a couple of pins
  pinMode(13, INPUT); // Read the lever press
  pinMode(12, OUTPUT); // Turn an LED ON if there is a lever press


}

void loop() {

   Alarm.timerOnce(5, deliver_reward);
   
  // Check the state of the lever
  lever_state = digitalRead(13);

  // If Lever press, Turn LED ON
  if (lever_state > 0) {
    digitalWrite(12, HIGH);
    Serial.println("LEVERPRESS: ON");
  }
  
  // No lever press, LED OFF
  digitalWrite(12, LOW);
  
}
