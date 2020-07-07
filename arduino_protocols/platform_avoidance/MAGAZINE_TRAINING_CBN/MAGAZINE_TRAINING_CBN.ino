
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu


MAGAZINE_TRAINING_COUL
----------------------
This protocols is build to habituate a subject to the food dispenser in the Coulbourn Chambers0. 


 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>


const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 12;        // LED in the chamber food tray 
const int session_len = 25;          // Length of the Session
const int acclimation_len = 5;       // Len in seconds.


// initialize the stepper library
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

void setup() {
  myStepper.setSpeed(70); // Speed of the stepper.
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println("LEDOUX LAB");
  Serial.println("MAGAZINE_TRAINING");
  Serial.println("STARTING IN (sec): 10");
  delay(10 * 1000L);
  
}

void loop() {
  Serial.println("SESSION: START");
  delay(1000);
  
  // ACCLIMATION
  Serial.print("ACCLIMATION (SEC): ");
  Serial.println(acclimation_len);
  delay(acclimation_len * 1000L);

  // DELIVER FOOD PELLET EACH ONE MINUTE FOR n MINUTES
  for (int x = 1; x < session_len; x++) {
  Serial.println("MAGAZINE: ON");
  myStepper.step(stepsPerRevolution/2);            // EQUIVALENT TO 1/2 PELLETS
  delay(60*1000L);
  Serial.print("REMAINING MINUTES: ");
  Serial.println(session_len - x);
  }
  
  Serial.println("SESSION: END");
  
  // ENDLESS LOOP TO ALLOW EXPERIMENTER TO SWITCH SUBJECT AND RESTART
  while(1) { }
}
