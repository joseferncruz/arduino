
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu

LEVER_PRESS_TRAINING_CBN
------------------------
Log lever presses and deliver food pellets when due.

*/


// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>

// VI AND VR
/*##################################################################################*/
unsigned long session_length = 25 * 60 * 1000L;  // DURATION OF THE SESSION >> "MIN * SEC * MS"
int max_vr = 4;                                  // MAX VARIABLE RATIO FOR RANDOM GENERATOR
int max_vi = 30;                                   // MAX VARIABLE INTERVAL FOR RANDOM GENERATOR

int feeder_type = 1;                              // 1 > MEDASSOCIATE 2 > COULDBOURN

unsigned long acclimation_length = 3;                        // DURATION IN MIN
unsigned long cooldown_length = 3;                           // DURATION IN MIN


// VI AND VR - STARTING VALUE (FOR FIRST TRIAL)
/*##################################################################################*/
unsigned long variable_interval = 1 * 1000L;                 // STARTING VALUE FOR VI
int variable_ratio = 1;                                      // STARTING VALUE FOR VR

// CONTROL TRANSITION BETWEEN VI AND VR
/*##################################################################################*/
unsigned long previous_time = millis();
bool access = false;                             // CONTROL THE TRANSITION TO VR


// TO KEEP TRACK OF LP/MIN
/*##################################################################################*/
unsigned long START = millis();
unsigned long INTERVAL = 60*1000L;


// KEEP TRACK OF MEASUREMENTS AND ALLOW BASIC LP STATS
/*##################################################################################*/
int LP_MIN = 0;
int LP = 0;
int LP_MINS = 0;
int LP_AVG= 0;
int N_PELLETS = 0;


// TRIAL INFORMATION
/*##################################################################################*/
int TRIAL_N = 50;
bool TRIAL_START = true; // acts as boolean


// SETTINGS FOR BOARD AND STEPPER - DO NOT MODIFY
/*##################################################################################*/
const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 8;         // LED in the chamber food tray
const int lever_press = 9;           // LEVER_PRESS DECTECTOR
bool alternate_spin;
int factor;

// SETTINGS FOR LEVER STEPPER 
/*##################################################################################*/
const int lp_stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
Stepper lp_myStepper(lp_stepsPerRevolution, 22, 24, 26, 28);

// 
bool lp_access = true;

// PINS TO READ
const int lp_inside_pin = 53;
const int lp_outside_pin = 52;

// VARIABLES TO READ

int lp_inside = 0;
int lp_outside = 0;
int lp_state_switch = 0;



// VARIABLES FOR LEVER PRESSING
/*##################################################################################*/
int lever_state = 0;
int press_lapse = 0;
int counting_presses = 0;
int cumsum_presses = 0;

// PUSH BUTTON TO START EXPERIMENT
/*##################################################################################*/
const int push_button = 2;

// PUSH BUTTON TO TEST FEEDER
/*##################################################################################*/
const int push_button_test_feeder = 3;

// INITIALIZE STEPPER (REVOLUTIONS ACCORDING TO FEEDER):
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);


void setup() {

  // SET STEPPER SPEED
  myStepper.setSpeed(100); // STEPPER SPEED

  // LEVER STEPPER
  lp_myStepper.setSpeed(60);
  
  // INITIALIZE SERIAL
  Serial.begin(9600);

  // PRINT INITIAL INFORMATION

  Serial.println("LEDOUX LAB");
  Serial.print("LEVER_PRESS_TRAINING"); Serial.print(" | VI"); Serial.print(max_vi); Serial.print(" | VR0"); Serial.println(max_vr);

  // ENSURE THAT LEVER IS RETRACTED
  lp_inside = digitalRead(lp_inside_pin);
  lp_outside = digitalRead(lp_outside_pin);

  if (lp_outside == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
        while (lp_inside != 1) {
          lp_myStepper.step(-1);
          //delay(20);
          lp_inside = digitalRead(lp_inside_pin);
          lp_outside = digitalRead(lp_outside_pin);
        }
  }
  
  Serial.println("PRESS GREEN BUTTON TO START");

  // SET UP PINS
  pinMode(food_tray_led, OUTPUT);
  pinMode(lever_press, INPUT);
  pinMode(push_button, INPUT);
  pinMode(push_button_test_feeder, INPUT);
  // LP STEPPER PINS
  pinMode(lp_inside_pin, INPUT);
  pinMode(lp_outside_pin, INPUT);

  // ENSURE REPRODUCIBILITY
  unsigned long seed = 31;
  randomSeed(seed);


// SPECIFY FEEDER TYPE
if (feeder_type == 1) {                         // MED ASSOCIATE FEEDER
  Stepper myStepper(48, 10, 11, 12, 13);
  } else if (feeder_type == 2) {                // COULBOURN FEEDER
    Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);
  }

}

void loop() {

  // START TRIAL
  int button_state = 0;

  // TEST FEEDER
  /*##################################################################################*/ 
  int push_button_test_feeder_state = digitalRead(push_button_test_feeder);
  
  if (push_button_test_feeder_state > 0) {


    // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
    if (feeder_type == 1) {             // 1 REPRESENTS MEDASSOCIATES
    for (int i = 0; i <4; i++) {
      myStepper.step(1);
      // ADD SMALL DELAY
      unsigned long start_ = millis();
      unsigned long current_ = millis();
      while ((current_ - start_) < 30) {
        
        // DO NOTHING
        current_ = millis();
      }

    }
    } else if (feeder_type == 2) {      // 2 REPRESENTS COULBOURN
      
      myStepper.step(stepsPerRevolution/4);
    }
    
    Serial.println("MANUAL FEEDER");
    
    // ADD SMALL DELAY
    unsigned long start_ = millis();
    unsigned long delay_ = millis();
    while (delay_ - start_ < 1000) {
      // DO NOTHING
      delay_ = millis(); 
  }
  }
  
  // PRESS BUTTON FOR ONE SECOND OR GET INPUT FROM ARDUINO 02 FOR ONE SECOND
  /*##################################################################################*/
  button_state = digitalRead(push_button);
  if (button_state == HIGH) {
    
    unsigned long press_button_start = millis();
    unsigned long press_button_current = millis();

    // LOOP FOR ONE SECOND 
    while (press_button_current - press_button_start < 1000L) {
      press_button_current = millis();
      button_state = digitalRead(push_button); // IF STATE IS STILL HIGH AFTER ONE SECOND, INITIATE THE FOLLOWING LOOP 
    }
  }
    
  unsigned long session_start_time = millis();
  unsigned long session_current_time = millis();

  int session_status = 1;              // WHEN ZERO THEN SESSION IS OVER

  if (button_state == HIGH) {
    // DISPLAY SESSION INFORMATION
    /*##################################################################################*/
    Serial.println("SESSION: LEVER PRESS TRAINING");
    
    Serial.print("SESSION LENGHT (SEC): "); Serial.println(session_length / (1000L));

    Serial.print("STARTING VI (SEC): "); Serial.print(variable_interval / 1000); Serial.print(" | STARTING VR: "); Serial.println(variable_ratio);
    
    Serial.print("SESSION VI (SEC): "); Serial.print(max_vi); Serial.print(" | SESSION VR: "); Serial.println(max_vr);
      
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(acclimation_length*60);

    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(cooldown_length*60);
    
    Serial.println("SESSION: START");


    // LOOP UNTIL THE SESSION IS OVER (TIME LIMIT)
    /*##################################################################################*/
    while (session_status == 1) {

          // TEST FEEDER
          /*##################################################################################*/ 
          int push_button_test_feeder_state = digitalRead(push_button_test_feeder);
          
          if (push_button_test_feeder_state > 0) {
                
                // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
                if (feeder_type == 1) {             // 1 REPRESENTS MEDASSOCIATES
                for (int i = 0; i <4; i++) {
                  myStepper.step(1);
                  // ADD SMALL DELAY
                  unsigned long start_ = millis();
                  unsigned long current_ = millis();
                  while ((current_ - start_) < 30) {
                    
                    // DO NOTHING
                    current_ = millis();
                  }
            
                }
                } else if (feeder_type == 2) {      // 2 REPRESENTS COULBOURN
                  myStepper.step(stepsPerRevolution/4);
                }
            Serial.println("MANUAL-FEEDER > ON");
            
            // ADD SMALL DELAY
            unsigned long start_ = millis();
            unsigned long delay_ = millis();
            
            while (delay_ - start_ < 1000) {
              // DO NOTHING
              delay_ = millis(); 
          }
            
          }

          // ACCLIMATION PERIOD
          /*##################################################################################*/
          if (TRIAL_START == true) {

            // ENSURE THAT LEVER IS RETRACTED
            lp_inside = digitalRead(lp_inside_pin);
            lp_outside = digitalRead(lp_outside_pin);

            if (lp_outside == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                  while (lp_inside != 1) {
                    lp_myStepper.step(-1);
                    //delay(20);
                    lp_inside = digitalRead(lp_inside_pin);
                    lp_outside = digitalRead(lp_outside_pin);
                  }
            }

            Serial.print("ACCLIMATION (SEC): "); Serial.println(acclimation_length*60);
            
            unsigned long interval_acclimation = acclimation_length*60*1000L;
            unsigned long start_acclimation = millis();
            unsigned long current_acclimation = millis();
            
            // KEEP ARDUINO IN A LOOP TO SIMULATE THE DELAY
            while (current_acclimation - start_acclimation < interval_acclimation) {
              current_acclimation = millis();
            }

            TRIAL_START = false;
            START = millis();                        // VARIABLE USED TO CALCULATE LP/MIN
            Serial.println("ACCLIMATION: OFF");
          }


          // ENTER COOLDOWNSTOP AND STOP SESSION AFTER TIME LIMIT
          /*##################################################################################*/
          if (session_current_time - session_start_time < session_length) {
            // DO NOTHING
            session_current_time = millis();
          } else {
            session_status == 0;

            // ENSURE THAT LEVER IS RETRACTED
            lp_inside = digitalRead(lp_inside_pin);
            lp_outside = digitalRead(lp_outside_pin);

            if (lp_outside == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                  while (lp_inside != 1) {
                    lp_myStepper.step(-1);
                    //delay(20);
                    lp_inside = digitalRead(lp_inside_pin);
                    lp_outside = digitalRead(lp_outside_pin);
                  }
            }
            
            // ENTER COOLDOWN
            
            Serial.print("COOLDOWN (SEC): "); Serial.println(cooldown_length*60);
            delay(cooldown_length*60*1000L);
            
            // STOP SESSION
            Serial.println("SESSION: END");
            Serial.print("GLOBAL LP/MIN: "); Serial.println(cumsum_presses/(session_length/60000L));
            Serial.print("CUMULATIVE LP: "); Serial.println(cumsum_presses);
            Serial.print("CUMULATIVE N PELLETS: "); Serial.println(N_PELLETS);
            while(1){}                                   // ENTER INFINITE LOOP TO ALLOW USER TO RESET BOARD
          }
          
          // ALLOW ACCESS TO THE LEVER
          /*##################################################################################*/
          lp_inside = digitalRead(lp_inside_pin);
          lp_outside = digitalRead(lp_outside_pin);

          if (lp_inside == 1) {
            while (lp_outside != 1) {
              lp_myStepper.step(1);
              ///delay(20);
              lp_outside = digitalRead(lp_outside_pin);
              lp_inside = digitalRead(lp_inside_pin);
            }
          }

          // CALCULATE LEVER PRESSES PER MINUTE
          /*########################################
          ##########################################*/
          unsigned long CURRENT = millis();
          if (CURRENT - START >= INTERVAL) {

            // ADD ONE MINUTE TO COUNTER
            LP_MINS ++;

            // RE-ASSIGN START MILLIS
            START = millis();

            // PRINT LP MEASUREMENTS
            LP_AVG = LP / LP_MINS;
            Serial.print("GLOBAL LP/MIN: "); Serial.println(LP_AVG);
            Serial.print("LP/MIN - PREVIOUS MINUTE: "); Serial.println(LP_MIN);
            Serial.print("CUMULATIVE LP: "); Serial.println(cumsum_presses);
            Serial.print("CUMULATIVE N PELLETS: "); Serial.println(N_PELLETS);
            Serial.print("ELAPSED TIME (SEC): "); Serial.println((session_current_time - session_start_time)/(1000L));
            LP_MIN = 0;
          }


          // GET CURRENT TIME
          unsigned long current_time = millis();


          // CODE TO CONTROL FEEDER AND LEVER PRESS
          /*##################################################################################*/

          // CHECK IF VI IS OVER
          if (current_time - previous_time <= variable_interval) {
            
            // IF NO THEN
            access = false;                                // ACCESS TO VR == NUMBER OF LP NECESSARY TO ACTIVATE THE MAGAZINE

          } else {
            access = true;                                 // ALLOW THE LEVER PRESSES TO COUNT TOWARDS VR
          }

          // ACCESS == TRUE > INITIATE VR
          if (access == true) {

          // COUNT LEVER PRESSES NECESSARY FOR THE APPROPRIATE VR AND ACTIVATE MAGAZINE
          /*##################################################################################*/
          if (counting_presses < variable_ratio){

                  // DETECT LEVER STATE
                  lever_state = digitalRead(lever_press);

                  // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                  if ((lever_state == HIGH) && (press_lapse == 0)) {
                    
                    press_lapse = 1;
                    Serial.println("LEVER > ON");

                    // KEEP TRACK OF MEASUREMENTS
                    LP ++;
                    cumsum_presses ++;
                    LP_MIN ++;

                    // ADD SMALL DELAY 0.2s TO AVOID LP ARTIFACTS
                    unsigned long start_ = millis();
                    unsigned long ongoing_ = millis();
                    while (ongoing_ - start_ < 200) {
                      // DO NOTHING
                      ongoing_ = millis();
                    }

                    // KEEP TRACK OF THE NUMBER OF LEVER PRESSES
                    counting_presses ++;

                  } else if ((lever_state == HIGH) && (press_lapse == 1)){
                    // DO NOTHING
                  } else if ((lever_state == LOW) && (press_lapse == 1)) {
                    press_lapse = 0;
                  } else {}

          } else {
            
            // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
            if (feeder_type == 1) {             // 1 REPRESENTS MEDASSOCIATES
            
            for (int i = 0; i <4; i++) {

              myStepper.step(1);
              // ADD SMALL DELAY
              unsigned long start_ = millis();
              unsigned long current_ = millis();
              while ((current_ - start_) < 30) {
                
                // DO NOTHING
                current_ = millis();
              }
        
            }
            } else if (feeder_type == 2) {      // 2 REPRESENTS COULBOURN
              myStepper.step(stepsPerRevolution/4);
            }
               
            Serial.println("FEEDER > ON");
            N_PELLETS ++;
            
         // RESET COUNTING PRESSES FOR VR
            counting_presses = 0;

            // RESET THE VR FOR NEXT TRIAL
            variable_ratio = random(1, max_vr+1);
            Serial.print("NEW VR (LP): "); Serial.println(variable_ratio);

            // RE-INITIATE VI
            access = false;                    // DETERMINES ACCESS TO MAGAZINE
            previous_time = millis();
            variable_interval = random(1, max_vi+1) * 1000L;
            Serial.print("NEXT VI (SEC): "); Serial.println(variable_interval / 1000);
          }

          // LOG LEVER PRESSES DURING VI30
          /*##################################################################################*/
          } else { 

                    // DETECT LEVER STATE AND COUNT THE PRESS
                    /*##################################################################################*/
                    lever_state = digitalRead(lever_press);

                    // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                    if ((lever_state == HIGH) && (press_lapse == 0)) {
                      press_lapse = 1;
                      Serial.println("LEVER > ON");

                      // KEEP TRACK OF MEASUREMENTS
                      LP ++;
                      cumsum_presses ++;
                      LP_MIN ++;
                      
                      // ADD SMALL DELAY 0.2s TO AVOID LP ARTIFACTS
                      unsigned long start_ = millis();
                      unsigned long ongoing_ = millis();
                      while (ongoing_ - start_ < 200) {
                        // DO NOTHING
                        ongoing_ = millis();
                      }

                    } else if ((lever_state == HIGH) && (press_lapse == 1)){
                    // DO NOTHING
                    } else if ((lever_state == LOW) && (press_lapse == 1)) {
                      press_lapse = 0;
                    } else {}

          }

  }} else {
    // do nothing
      }
}
