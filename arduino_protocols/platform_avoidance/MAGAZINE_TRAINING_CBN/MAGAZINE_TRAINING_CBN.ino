
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu


MAGAZINE_TRAINING_COUL
----------------------
Deliver one food pellet per minute. 


 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>


const int stepsPerRevolution = 200;     // steps per revolution
const int food_tray_led = 8;            // LED in the chamber food tray 
const int push_button = 2;              // push button to start the protocol
const int acclimation_len = 3*60;       // Len in seconds. Same as cooldown
const int magazine_trials = 10;         // Number of times the magazine will be activated 


// initialize the stepper library
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

void setup() {
  myStepper.setSpeed(70); // Speed of the stepper.
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(food_tray_led, OUTPUT);
  pinMode(push_button, INPUT);
  Serial.println("LEDOUX LAB");
  Serial.println("MAGAZINE_TRAINING: ONE PELLET/MIN FOR TEN MINUTES");
  Serial.println("PRESS GREEN BUTTON TO START");
  
}

void loop() {
  
  int button_state = 0;
  button_state = digitalRead(push_button);
  
  if (button_state == HIGH) {
    
    Serial.println("SESSION: START");
    delay(1000);
    
    // ACCLIMATION
    Serial.print("ACCLIMATION (SEC): ");
    Serial.println(acclimation_len);
    delay(acclimation_len * 1000L);
  
    // DELIVER ONE FOOD PELLET EACH ONE MINUTE FOR N MINUTES
    for (int x = 1; x < magazine_trials; x++) {
    Serial.println("MAGAZINE: ON");
    myStepper.step(stepsPerRevolution/2);            // EQUIVALENT TO 1/2 PELLETS
    digitalWrite(food_tray_led, HIGH);
    Serial.print("REMAINING MINUTES: ");
    Serial.println(magazine_trials - x);
    delay(3*1000L);
    digitalWrite(food_tray_led, LOW);
    delay(57*1000L);
    }
  
    // COOLDOWN
    Serial.print("COOLDOWN (SEC): ");
    Serial.println(acclimation_len);
    delay(acclimation_len * 1000L);
    Serial.println("SESSION: END");
    
  }
}
