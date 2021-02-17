
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu


MAGAZINE_TRAINING_COUL
----------------------
Deliver one food pellet per minute. 


 */

// LIBRARY TO CONTROL STEPPER
#include <Stepper.h>



// CONSTANTS TO SET BY USER (DEFAULT BELOW)
/*####################################################################################*/
const int acclimation_len = 3 * 60L;        // ACCLIMATION LENGTH IN SECONDS
const int cooldown_len = 3 * 60L;           // COOLDOWN LENGTH IN SECONDS
const int magazine_trials = 15;             // NUMBER OF TIMES THE STEPPER ROTATES.





// CONSTANTS
/*####################################################################################*/
const int stepsPerRevolution = 200;         // STEPS PER REVOLUTION - COULBOURN STEPPER
const int food_tray_led = 8;                // FOOD TRAY LED
const int push_button = 2;                  // START  SWITCH -  EXPERIMENT


// INSTANTIATE STEPPER OBJECT
/*####################################################################################*/
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);




void setup() {
  myStepper.setSpeed(70);                   // SPEED OF THE STEPPER
  
  
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(food_tray_led, OUTPUT);
  pinMode(push_button, INPUT);
  Serial.println("LEDOUX LAB");
  Serial.print("MAGAZINE TRAINING");
  Serial.println("PRESS GREEN BUTTON TO START");
  
}

void loop() {

  // TEST CHAMBER LED




  // TEST MAGAZINE



  // START NEW EXPERIMENTAL SESSION
  /*####################################################################################*/

  int button_state = 0;
  // READ START SWITCH
  button_state = digitalRead(push_button);

  // IF SIGNAL IS DETECTED THEN...
  /*####################################################################################*/
  if (button_state == HIGH) {

    // PRINT SESSION INFORMATION
    /*##################################################################################*/
    Serial.println("SESSION: MAGAZINE TRAINING");
    delay(1000);

    Serial.println("NUMBER OF PELLETS/MINUTE: 01"); 
    delay(500);

    Serial.print("NUMBER OF TRIALS: "); Serial.println(magazine_trials); 
    delay(500);

    Serial.print("ACCLIMATION TIME (SEC): ");
    Serial.println(acclimation_len);
    delay(500);    

    Serial.print("COOLDOWN TIME (SEC): ");
    Serial.println(cooldown_len);
    delay(500);

    // SIGNAL START OF THE NEW EXPERIMENT
    /*##################################################################################*/
    Serial.println("NEW EXPERIMENT: START");
    delay(1000);        
    
  
    // ACCLIMATION
    /*##################################################################################*/
    Serial.print("ACCLIMATION (SEC): "); Serial.println(acclimation_len);
    delay(acclimation_len * 1000L);

    
  
    // DELIVER ONE FOOD PELLET EACH ONE MINUTE FOR N MINUTES
    /*##################################################################################*/
    for (int x = 1; x < magazine_trials; x++) {
      
        Serial.println("MAGAZINE > ON");
        myStepper.step(stepsPerRevolution/4);            // EQUIVALENT TO 1/2 PELLETS
        digitalWrite(food_tray_led, HIGH);               // FOOD LED ON

        Serial.print("REMAINING TRIALS: "); Serial.println(magazine_trials - x);
        delay(3*1000L);
        digitalWrite(food_tray_led, LOW);                // FOOD LED OFF
        delay(57*1000L);
    
    }

    // COOLDOWN
    /*##################################################################################*/
    Serial.print("COOLDOWN (SEC): "); Serial.println(cooldown_len);
    
    delay(cooldown_len * 1000L);
    
    Serial.println("NEW EXPERIMENT: END");
    
  }
}
