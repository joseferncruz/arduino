
/*
LeDoux Lab | 2021
Jose Oliveira da Cruz, jose [dot] cruz [at] nyu [dot] edu

LEVER_PRESS_TRAINING_CBN_VI_VR
------------------------------
Log lever presses and deliver food pellets when due.

*/



// SESSION AND TRIAL INFO >> TO MODIFY
/*##################################################################################*/

unsigned long SESSION_LENGTH_MIN = 25;
unsigned long ACCLIMATION_LENGHT_MIN = 1;      
unsigned long COOLDOWN_LENGHT_MIN = 1;
          int FEEDER_TYPE = 2;                    // 1 > MEDASSOCIATE 2 > COULDBOURN
          int MAX_VR_LP = 4;         
          int MAX_VI_SEC = 15;
          
// ###################################################################################
// ###################################################################################
// -------------- DO NOT MODIFY --------------------------------------------------- //
// ###################################################################################
// ###################################################################################

// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>

// SETTINGS FOR PINS AND STEPPER MOTORS (FEEDER AND LEVER)
/*##################################################################################*/
const int START_SWITCH_PIN_GREEN = 2;
const int MANUAL_FEEDER_SWITCH_PIN_YELLOW = 3;
const int CS_LED_PIN = 5;
const int LEVER_PIN = 9;
const int FEEDER_STEPS_PER_REVOLUTION = 200;  // steps per revolution
Stepper FEEDER_STEPPER(FEEDER_STEPS_PER_REVOLUTION, 10, 11, 12, 13);
const int LEVER_STEPS_PER_REVOLUTION = 200;  // change this to fit the number of steps per revolution
Stepper LEVER_STEPPER(LEVER_STEPS_PER_REVOLUTION, 22, 24, 26, 28);
const int LEVER_OUT_DETECTOR_PIN = 52;
const int LEVER_IN_DETECTOR_PIN = 53;
  
// REINFORCEMENT SCHEDULE - VI AND VR - STARTING VALUE
/*##################################################################################*/
unsigned long CURRENT_VI = 1 * 1000L;          
int CURRENT_VR = 1;
unsigned long previous_time = millis(); // CONTROL TRANSITION BETWEEN Vi
bool COUNT_TOWARDS_VR = false;          // CONTROL THE TRANSITION TO VR

// VARIABLES LEVER PRESS STATISTICS
/*##################################################################################*/
unsigned long START = millis();
unsigned long INTERVAL = 60*1000L;
int LP_MIN = 0;
int LP = 0;
int LP_MINS = 0;
int LP_AVG= 0;
int N_PELLETS = 0;

// TRIAL CONTROL FLOW
/*#################################S#################################################*/
bool SESSION_START = true; // Control start of the session
int START_SWITCH_STATE;

// LEVER VARIABLES
/*##################################################################################*/
int IS_LEVER_INSIDE = 0;
int IS_LEVER_OUTSIDE = 0;
int LEVER_STATE = 0;
int press_lapse = 0;
int LP_TOWARDS_CURRENT_VR_THRESHOLD = 0;
int CUMULATIVE_SUM_LP = 0;


void setup() {

  // SET STEPPER SPEED
  FEEDER_STEPPER.setSpeed(100);
  LEVER_STEPPER.setSpeed(60);
  
  // INITIALIZE SERIAL
  Serial.begin(9600);

  // PRINT INITIAL INFORMATION
  Serial.println("LEDOUX LAB");
    Serial.print("LEVER_PRESS_TRAINING");
    Serial.print(" | VI"); Serial.print(MAX_VI_SEC);
    Serial.print(" | VR0"); Serial.println(MAX_VR_LP);

  // ENSURE THAT LEVER IS RETRACTED
  IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
  IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);

  if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE MOVE IT INSIDE
        while (IS_LEVER_INSIDE != 1) {
          LEVER_STEPPER.step(-1);
          //delay(20);
          IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
          IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
        }
  }
  
  Serial.println("PRESS GREEN BUTTON TO START");

  // SET UP PINS
  pinMode(LEVER_PIN, INPUT);
  pinMode(START_SWITCH_PIN_GREEN, INPUT);
  pinMode(CS_LED_PIN, OUTPUT);
  pinMode(MANUAL_FEEDER_SWITCH_PIN_YELLOW, INPUT);
  pinMode(LEVER_IN_DETECTOR_PIN, INPUT);
  pinMode(LEVER_OUT_DETECTOR_PIN, INPUT);
  
  // ENSURE REPRODUCIBILITY
  unsigned long seed = 31;
  randomSeed(seed);

// SPECIFY FEEDER TYPE
if (FEEDER_TYPE == 1) {                         // MED ASSOCIATE FEEDER
  Stepper FEEDER_STEPPER(48, 10, 11, 12, 13);
  } else if (FEEDER_TYPE == 2) {                // COULBOURN FEEDER
    Stepper FEEDER_STEPPER(FEEDER_STEPS_PER_REVOLUTION, 10, 11, 12, 13);
  }

}

void loop() {

  // START TRIAL
  START_SWITCH_STATE = 0;

  // TEST FEEDER
  /*##################################################################################*/ 
  int FEEDER_SWITCH_STATE = digitalRead(MANUAL_FEEDER_SWITCH_PIN_YELLOW);
  
  if (FEEDER_SWITCH_STATE == HIGH) {


    // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
    if (FEEDER_TYPE == 1) {             // 1 REPRESENTS MEDASSOCIATES
    for (int i = 0; i <4; i++) {
      FEEDER_STEPPER.step(1);
      // ADD SMALL DELAY
      unsigned long start_ = millis();
      unsigned long current_ = millis();
      while ((current_ - start_) < 30) {
        
        // DO NOTHING
        current_ = millis();
      }

    }
    } else if (FEEDER_TYPE == 2) {      // 2 REPRESENTS COULBOURN
      
      FEEDER_STEPPER.step(FEEDER_STEPS_PER_REVOLUTION/4);
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
  START_SWITCH_STATE = digitalRead(START_SWITCH_PIN_GREEN);
  if (START_SWITCH_STATE == HIGH) {
    
    unsigned long press_button_start = millis();
    unsigned long press_button_current = millis();

    // LOOP FOR ONE SECOND 
    while (press_button_current - press_button_start < 1000L) {
      press_button_current = millis();
      START_SWITCH_STATE = digitalRead(START_SWITCH_PIN_GREEN); // IF STATE IS STILL HIGH AFTER ONE SECOND, INITIATE THE FOLLOWING LOOP 
    }
  }
    
  unsigned long session_start_time = millis();
  unsigned long session_current_time = millis();

  int session_status = 1;              // WHEN ZERO THEN SESSION IS OVER

  if (START_SWITCH_STATE == HIGH) {
    // DISPLAY SESSION INFORMATION
    /*##################################################################################*/
    Serial.println("SESSION: LEVER PRESS TRAINING");

    // BLINK LED TO SIGNAL THE START OF THE SESSION
    for (int i = 0; i < 5; i++) {
      digitalWrite(CS_LED_PIN, HIGH);
      delay(500);
      digitalWrite(CS_LED_PIN, LOW);    
      delay(500);  
    }
    
    Serial.print("SESSION LENGHT (SEC): "); Serial.println(SESSION_LENGTH_MIN * 60);

    Serial.print("STARTING VI (SEC): "); Serial.print(CURRENT_VI / 1000); Serial.print(" | STARTING VR: "); Serial.println(CURRENT_VR);
    
    Serial.print("SESSION VI (SEC): "); Serial.print(MAX_VI_SEC); Serial.print(" | SESSION VR: "); Serial.println(MAX_VR_LP);
      
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(ACCLIMATION_LENGHT_MIN*60);

    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(COOLDOWN_LENGHT_MIN*60);
    
    Serial.println("SESSION: START");


    // LOOP UNTIL THE SESSION IS OVER (TIME LIMIT)
    /*##################################################################################*/
    while (session_status == 1) {

          // TEST FEEDER
          /*##################################################################################*/ 
          int FEEDER_SWITCH_STATE = digitalRead(MANUAL_FEEDER_SWITCH_PIN_YELLOW);
          
          if (FEEDER_SWITCH_STATE == HIGH) {
                
                // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
                if (FEEDER_TYPE == 1) {             // 1 REPRESENTS MEDASSOCIATES
                for (int i = 0; i <4; i++) {
                  FEEDER_STEPPER.step(1);
                  // ADD SMALL DELAY
                  unsigned long start_ = millis();
                  unsigned long current_ = millis();
                  while ((current_ - start_) < 30) {
                    
                    // DO NOTHING
                    current_ = millis();
                  }
            
                }
                } else if (FEEDER_TYPE == 2) {      // 2 REPRESENTS COULBOURN
                  FEEDER_STEPPER.step(FEEDER_STEPS_PER_REVOLUTION/4);
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
          if (SESSION_START == true) {

            // ENSURE THAT LEVER IS RETRACTED
            IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
            IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);

            if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                  while (IS_LEVER_INSIDE != 1) {
                    LEVER_STEPPER.step(-1);
                    //delay(20);
                    IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                    IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                  }
            }

            Serial.print("ACCLIMATION (SEC): "); Serial.println(ACCLIMATION_LENGHT_MIN*60);
            
            unsigned long interval_acclimation = ACCLIMATION_LENGHT_MIN*60*1000L;
            unsigned long start_acclimation = millis();
            unsigned long current_acclimation = millis();
            
            // KEEP ARDUINO IN A LOOP TO SIMULATE THE DELAY
            while (current_acclimation - start_acclimation < interval_acclimation) {
              current_acclimation = millis();
            }

            SESSION_START = false;
            START = millis();                        // VARIABLE USED TO CALCULATE LP/MIN
            Serial.println("ACCLIMATION: OFF");
            session_start_time = millis();
            session_current_time = millis();
          }


          // ENTER COOLDOWNSTOP AND STOP SESSION AFTER TIME LIMIT
          /*##################################################################################*/
          if (session_current_time - session_start_time < (SESSION_LENGTH_MIN*60*1000L)) {
            // DO NOTHING
            session_current_time = millis();
          } else {
            session_status == 0;

            // ENSURE THAT LEVER IS RETRACTED
            IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
            IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);

            if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                  while (IS_LEVER_INSIDE != 1) {
                    LEVER_STEPPER.step(-1);
                    //delay(20);
                    IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                    IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                  }
            }
            
            // ENTER COOLDOWN
            
            Serial.print("COOLDOWN (SEC): "); Serial.println(COOLDOWN_LENGHT_MIN*60);
            delay(COOLDOWN_LENGHT_MIN*60*1000L);
            
            // STOP SESSION
            Serial.println("SESSION: END");
            Serial.print("GLOBAL LP/MIN: "); Serial.println(CUMULATIVE_SUM_LP/SESSION_LENGTH_MIN);
            Serial.print("CUMULATIVE LP: "); Serial.println(CUMULATIVE_SUM_LP);
            Serial.print("CUMULATIVE N PELLETS: "); Serial.println(N_PELLETS);
            while(1){}                                   // ENTER INFINITE LOOP TO ALLOW USER TO RESET BOARD
          }
          
          // ALLOW ACCESS TO THE LEVER
          /*##################################################################################*/
          IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
          IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);

          if (IS_LEVER_INSIDE == 1) {
            while (IS_LEVER_OUTSIDE != 1) {
              LEVER_STEPPER.step(1);
              ///delay(20);
              IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
              IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
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
            Serial.print("CUMULATIVE LP: "); Serial.println(CUMULATIVE_SUM_LP);
            Serial.print("CUMULATIVE N PELLETS: "); Serial.println(N_PELLETS);
            Serial.print("ELAPSED TIME (SEC): "); Serial.println((session_current_time - session_start_time)/(1000L));
            LP_MIN = 0;
          }


          // GET CURRENT TIME
          unsigned long current_time = millis();


          // CODE TO CONTROL FEEDER AND LEVER PRESS
          /*##################################################################################*/

          // CHECK IF VI IS OVER
          if (current_time - previous_time <= CURRENT_VI) {
            
            // IF NO THEN
            COUNT_TOWARDS_VR = false;                                // COUNT_TOWARDS_VR TO VR == NUMBER OF LP NECESSARY TO ACTIVATE THE MAGAZINE

          } else {
            COUNT_TOWARDS_VR = true;                                 // ALLOW THE LEVER PRESSES TO COUNT TOWARDS VR
          }

          // COUNT_TOWARDS_VR == TRUE > INITIATE VR
          if (COUNT_TOWARDS_VR == true) {

          // COUNT LEVER PRESSES NECESSARY FOR THE APPROPRIATE VR AND ACTIVATE MAGAZINE
          /*##################################################################################*/
          if (LP_TOWARDS_CURRENT_VR_THRESHOLD < CURRENT_VR){

                  // DETECT LEVER STATE
                  LEVER_STATE = digitalRead(LEVER_PIN);

                  // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                  if ((LEVER_STATE == HIGH) && (press_lapse == 0)) {
                    
                    press_lapse = 1;
                    Serial.println("LEVER > ON");

                    // KEEP TRACK OF MEASUREMENTS
                    LP ++;
                    CUMULATIVE_SUM_LP ++;
                    LP_MIN ++;

                    // ADD SMALL DELAY 0.2s TO AVOID LP ARTIFACTS
                    unsigned long start_ = millis();
                    unsigned long ongoing_ = millis();
                    while (ongoing_ - start_ < 200) {
                      // DO NOTHING
                      ongoing_ = millis();
                    }

                    // KEEP TRACK OF THE NUMBER OF LEVER PRESSES
                    LP_TOWARDS_CURRENT_VR_THRESHOLD ++;

                  } else if ((LEVER_STATE == HIGH) && (press_lapse == 1)){
                    // DO NOTHING
                  } else if ((LEVER_STATE == LOW) && (press_lapse == 1)) {
                    press_lapse = 0;
                  } else {}

          } else {
            
            // DELIVER FOOD (N REVOLUTIONS IS SPECIFIC TO THE FEEDER)
            if (FEEDER_TYPE == 1) {             // 1 REPRESENTS MEDASSOCIATES
            
            for (int i = 0; i <4; i++) {

              FEEDER_STEPPER.step(1);
              // ADD SMALL DELAY
              unsigned long start_ = millis();
              unsigned long current_ = millis();
              while ((current_ - start_) < 30) {
                
                // DO NOTHING
                current_ = millis();
              }
        
            }
            } else if (FEEDER_TYPE == 2) {      // 2 REPRESENTS COULBOURN
              FEEDER_STEPPER.step(FEEDER_STEPS_PER_REVOLUTION/4);
            }
               
            Serial.println("FEEDER > ON");
            N_PELLETS ++;
            
         // RESET COUNTING PRESSES FOR VR
            LP_TOWARDS_CURRENT_VR_THRESHOLD = 0;

            // RESET THE VR FOR NEXT TRIAL
            CURRENT_VR = random(1, MAX_VR_LP+1);
            Serial.print("NEW VR (LP): "); Serial.println(CURRENT_VR);

            // RE-INITIATE VI
            COUNT_TOWARDS_VR = false;                    // DETERMINES COUNT_TOWARDS_VR TO MAGAZINE
            previous_time = millis();
            CURRENT_VI = random(1, MAX_VI_SEC+1) * 1000L;
            Serial.print("NEXT VI (SEC): "); Serial.println(CURRENT_VI / 1000);
          }

          // LOG LEVER PRESSES DURING VI30
          /*##################################################################################*/
          } else { 

                    // DETECT LEVER STATE AND COUNT THE PRESS
                    /*##################################################################################*/
                    LEVER_STATE = digitalRead(LEVER_PIN);

                    // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                    if ((LEVER_STATE == HIGH) && (press_lapse == 0)) {
                      press_lapse = 1;
                      Serial.println("LEVER > ON");

                      // KEEP TRACK OF MEASUREMENTS
                      LP ++;
                      CUMULATIVE_SUM_LP ++;
                      LP_MIN ++;
                      
                      // ADD SMALL DELAY 0.2s TO AVOID LP ARTIFACTS
                      unsigned long start_ = millis();
                      unsigned long ongoing_ = millis();
                      while (ongoing_ - start_ < 200) {
                        // DO NOTHING
                        ongoing_ = millis();
                      }

                    } else if ((LEVER_STATE == HIGH) && (press_lapse == 1)){
                    // DO NOTHING
                    } else if ((LEVER_STATE == LOW) && (press_lapse == 1)) {
                      press_lapse = 0;
                    } else {}

          }

  }} else {
    // do nothing
      }
}
