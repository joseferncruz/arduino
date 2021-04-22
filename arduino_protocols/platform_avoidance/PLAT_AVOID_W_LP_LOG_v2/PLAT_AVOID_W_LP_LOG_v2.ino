/*

LeDoux Lab | 2021
Jose Oliveira da Cruz, jose [dot] cruz [at] nyu [dot] edu

PLAT_AVOID_W_LP_LOG_v2
------------------------
Log lever presses and deliver food pellets when due.


*/

// SESSION AND TRIAL INFO >> TO MODIFY
/*##################################################################################*/
          int N_TRIALS = 3;                            
unsigned long TRIAL_LENGHT_MIN = 13;                    
unsigned long ACCLIMATION_LENGHT_MIN = 3;                  
unsigned long COOLDOWN_LENGHT_MIN = 2;                     
unsigned long LENGHT_WITH_LEVER_BEFORE_FIRST_CS = 5;  
          int INTER_TRIAL_INTERVAL_LIST[] = {120, 120, 120};  // IN SECONDS
              
// CS AND US VARIABLES >> TO MODIFY
/*##################################################################################*/
const int TOTAL_CS_NUMBER = 3;          // NUMBER OF CS
      int CS_LENGHT_SEC = 30;           // SECONDs
     bool TRIAL_WITH_US = true;         // IF FALSE THEN NO US
      int US_PAIRED_LENGTH_SEC = 2;     // PAIRED WITH THE LAST N SECONDS OF CS_LENGTH_SEC 

// REINFORCEMENT SCHEDULE - VI AND VR >> TO MODIFY
/*##################################################################################*/
int MAX_VR_LP = 4;                // MAX VARIABLE RATIO FOR RANDOM GENERATOR
int MAX_VI_SEC = 30;              // MAX VARIABLE INTERVAL IN SEC FOR RANDOM GENERATOR


// ###################################################################################
// ###################################################################################
// -------------- DO NOT MODIFY --------------------------------------------------- //
// ###################################################################################
// ###################################################################################

// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>


// SETTINGS FOR PINS AND STEPPER MOTORS (FEEDER AND LEVER)
/*##################################################################################*/
const int FEEDER_STEPS_PER_REVOLUTION = 200;    // steps per revolutions
const int START_SWITCH_PIN_GREEN = 2;
const int MANUAL_FEEDER_SWITCH_PIN_YELLOW = 3;
const int CS_PIN = 4;
const int CS_LED_PIN = 5;
const int US_PIN = 6;      
const int MANUAL_TEST_CS_US_LED_SWITCH_PIN_BLUE = 7;
const int LEVER_PIN = 9;                         // LEVER_PRESS DECTECTOR
Stepper myStepper(FEEDER_STEPS_PER_REVOLUTION, 10, 11, 12, 13);
const int LEVER_STEPS_PER_REVOLUTION = 200;  // change this to fit the number of steps per revolution
Stepper lp_myStepper(LEVER_STEPS_PER_REVOLUTION, 22, 24, 26, 28);
const int LEVER_OUT_DETECTOR_PIN = 52;
const int LEVER_IN_DETECTOR_PIN = 53;


// TEST MESSAGES
/*###################################################################################*/
String TEST_CS_MESSAGE = "TESTING AUDIO (CS+) GENERATION";
String TEST_US_MESSAGE = "TESTING SHOCK (US) GENERATION";
String TEST_CHAMBER_LED_MESSAGE = "TESTING CHAMBER LED";

// VI AND VR - STARTING VALUE (FOR FIRST TRIAL) 
/*##################################################################################*/
int CURRENT_VR = 1;                                      // STARTING VALUE FOR VR
unsigned long CURRENT_VI = 1 * 1000L;                 // STARTING VALUE FOR VI

// CONTROL TRANSITION BETWEEN VI AND VR
/*##################################################################################*/
unsigned long previous_time = millis();
bool COUNT_TOWARDS_VR = false;                             // CONTROL THE TRANSITION TO VR


// TO KEEP TRACK OF LP/MIN
/*##################################################################################*/
unsigned long START;
unsigned long INTERVAL = 60*1000L;

// KEEP TRACK OF MEASUREMENTS AND BASIC LEVER PRESS STATISTICS
/*##################################################################################*/
int LP_MIN = 0;
int LP = 0;
int LP_MINS = 0;
int LP_AVG= 0;
int N_PELLETS = 0;
bool ITI_PELLETS;

int TOTAL_LP_DURING_CS;
int TOTAL_LP_BEFORE_CS;


// TRIAL INFORMATION
/*##################################################################################*/
bool TRIAL_START = true; 
int SESSION_CURRENT_STATUS = 1;              // WHEN ZERO THEN SESSION IS OVER


// VARIABLES TO DELIVER CS
/*##################################################################################*/
bool DELIVER_CS = true;
bool IS_INTER_CS_DELAY = false;
unsigned long CS_START_TIME;
unsigned long CS_ONGOING_TIME;
unsigned long INTER_CS_DELAY_START;
unsigned long INTER_CS_DELAY_LENGTH;                       // SECONDS
bool DELIVER_US;


// LEVER VARIABLES
/*##################################################################################*/
int IS_LEVER_INSIDE = 0;
int IS_LEVER_OUTSIDE = 0;
int LEVER_STATE = 0;
int press_lapse = 0;
int LP_TOWARDS_CURRENT_VR_THRESHOLD = 0;
int CUMULATIVE_SUM_LP = 0;

// OTHER VARIABLES
/*##################################################################################*/
int button_state; 
int CS_NUMBER = 1

//##########################################################################################
void setup() {

  // SET STEPPER SPEED
  myStepper.setSpeed(80); // STEPPER SPEED

  // LEVER STEPPER
  lp_myStepper.setSpeed(60);
  
  // INITIALIZE SERIAL
  Serial.begin(9600);

  // PRINT INITIAL INFORMATION
  Serial.println("LEDOUX LAB");
  Serial.print("LEVER PRESS LOG"); Serial.print(" | VI"); Serial.print(MAX_VI_SEC); Serial.print(" | VR0"); Serial.println(MAX_VR_LP);
  Serial.println("PRESS GREEN BUTTON TO START");
  Serial.println("PRESS YELLOW BUTTON TO TEST FEEDER");
  Serial.println("PRESS BLUE BUTTON TO TEST CS, US AND CHAMBER LED");
  
  // ENSURE THAT LEVER IS RETRACTED
  IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
  IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);

  if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
        while (IS_LEVER_INSIDE != 1) {
          lp_myStepper.step(-1);
          //delay(20);
          IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
          IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
        }
  }

  // SET UP PINS
  pinMode(LEVER_PIN, INPUT);
  pinMode(START_SWITCH_PIN_GREEN, INPUT);
  pinMode(MANUAL_FEEDER_SWITCH_PIN_YELLOW, INPUT);
  
  // LP STEPPER PINS
  pinMode(LEVER_IN_DETECTOR_PIN, INPUT);
  pinMode(LEVER_OUT_DETECTOR_PIN, INPUT);

  // TO DELIVER CS
  pinMode(CS_PIN, OUTPUT);
  pinMode(CS_LED_PIN, OUTPUT);

  // TO DELIVER US
  pinMode(US_PIN, OUTPUT);

  // TEST CS and US
  pinMode(MANUAL_TEST_CS_US_LED_SWITCH_PIN_BLUE, OUTPUT);

  // ENSURE REPRODUCIBILITY
  int seed = 31;
  randomSeed(seed);

}

void loop() {

  // START TRIAL
  START_SWITCH_STATE = LOW;

  // TEST FEEDER
  /*##################################################################################*/ 
  int push_button_test_feeder_state = digitalRead(MANUAL_FEEDER_SWITCH_PIN_YELLOW);
  
  if (push_button_test_feeder_state > 0) {
    myStepper.step(FEEDER_STEPS_PER_REVOLUTION/4);
    
    // ADD SMALL DELAY
    unsigned long start_ = millis();
    unsigned long delay_ = millis();
    while (delay_ - start_ < 1000) {
      // DO NOTHING
      delay_ = millis(); 
  }
  }

  // TEST CS + US GENERATION AND TEST CHAMBER LED
  /*##################################################################################*/ 
  if (digitalRead(MANUAL_TEST_CS_US_LED_SWITCH_PIN_BLUE) == HIGH) {
   
    //  TEST CS
    Serial.println(TEST_CS_MESSAGE);
    delay(500);
    digitalWrite(CS_PIN, HIGH);
    delay(10000); 
    digitalWrite(CS_PIN, LOW);
    Serial.print(TEST_CS_MESSAGE); Serial.println(" > COMPLETE");
    delay(500);

    // TEST US
    Serial.println(TEST_US_MESSAGE);
    delay(500);
    digitalWrite(US_PIN, HIGH);
    delay(5000); 
    digitalWrite(US_PIN, LOW);
    Serial.print(TEST_US_MESSAGE); Serial.println(" > COMPLETE");
    delay(500);

    // TEST CHAMBER-CS LED
    Serial.println(TEST_CHAMBER_LED_MESSAGE);
    delay(500);
    // MAKE LED BLINK
    for (int i = 0; i < 5; i++) {
    
      digitalWrite(CS_LED_PIN, HIGH);
      delay(500);
       
      digitalWrite(CS_LED_PIN, LOW);    
      delay(500);  
    }
    Serial.print(TEST_CHAMBER_LED_MESSAGE); Serial.println(" > COMPLETE");
    delay(500);
  }
  
  
  // PRESS GREEN SWITCH FOR ONE SECOND IN ORDER TO START SESSION
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
   

  if (START_SWITCH_STATE == HIGH) {

    // KEEP TRACK OF GLOBAL VARIABLES
    int TOTAL_SESSION_LP = 0;
    int TOTAL_SESSION_PELLETS = 0;

    
    // DISPLAY SESSION INFORMATION
    /*##################################################################################*/
    Serial.println("SESSION: PLATFORM AVOIDANCE WITH LEVER PRESS LOG");

    // BLINK LED TO SIGNAL THE START OF THE SESSION
    for (int i = 0; i < 5; i++) {
      digitalWrite(CS_LED_PIN, HIGH);
      delay(500);
      digitalWrite(CS_LED_PIN, LOW);    
      delay(500);  
    }
    
    Serial.print("TRIAL LENGHT (SEC): "); Serial.println((TRIAL_LENGHT_MIN * 60));
    Serial.print("NUMBER OF TRIALS: "); Serial.println(N_TRIALS); 
    Serial.print("NUMBER OF CS+: "); Serial.println(TOTAL_CS_NUMBER);
    Serial.print("CS+ DURATION (SEC): "); Serial.println(CS_LENGHT_SEC);
    if (TRIAL_WITH_US == true) {
      Serial.print("NUMBER OF US: "); Serial.println(TOTAL_CS_NUMBER);
    }
    Serial.print("STARTING VI (SEC): "); Serial.print(CURRENT_VI / 1000); Serial.print(" | STARTING VR: "); Serial.println(CURRENT_VR);  
    Serial.print("SESSION VI (SEC): "); Serial.print(MAX_VI_SEC); Serial.print(" | SESSION VR: "); Serial.println(MAX_VR_LP);  
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(ACCLIMATION_LENGHT_MIN*60);
    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(COOLDOWN_LENGHT_MIN*60);
    Serial.println("SESSION > START");


    // INITIATE TRIALS
    /*##################################################################################*/
    for (int i = 1; i < (N_TRIALS + 1); i++) {


      // PRINT TRIAL INFORMATION
      Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" > START");
      
      SESSION_CURRENT_STATUS = 1;                            // IF==1 > SESSION IS "ON"
      
      // TRIAL TIME VARIABLES
      unsigned long trial_start_time;                // TRACK THE START OF THE TRIAL
      unsigned long trial_current_time;              // TRACK THE PROGRESSION OF THE TRIAL

      TRIAL_START = true;                            // FALSE==END  
      
      CS_NUMBER = 1;

      // TRACK LP PRE CS
      int pre_cs_lp_start = 0;
    
      // KEEP TRACK OF THE LP PRE-CS AND PERI-CS
      int lp_pre_cs_list[TOTAL_CS_NUMBER];
      int lp_peri_cs_list[TOTAL_CS_NUMBER];
      byte list_end = 0 ; // the end pointer

      

      // LOOP UNTIL THE TRIAL IS OVER (==TIME LIMIT)
      /*##################################################################################*/
      while (SESSION_CURRENT_STATUS == 1) {
  
            // TEST FEEDER
            /*##################################################################################*/ 
            int push_button_test_feeder_state = digitalRead(MANUAL_FEEDER_SWITCH_PIN_YELLOW);
            
            if (push_button_test_feeder_state > 0) {
              myStepper.step(FEEDER_STEPS_PER_REVOLUTION/4);
              Serial.println("FEEDER > ON");
              // ADD SMALL DELAY
              unsigned long start_ = millis();
              unsigned long delay_ = millis();
              while (delay_ - start_ < 1000) {
                // DO NOTHING
                delay_ = millis(); 
            }
              
            }
  
            // START ACCLIMATION PERIOD > LEVER IS RETRACTED
            /*##################################################################################*/
            if (TRIAL_START == true) {
  
              // ENSURE THAT LEVER IS RETRACTED DURING ACCLIMATION
              /*---------------------------------------------------------*/
              IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
              IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
  
              if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                    while (IS_LEVER_INSIDE != 1) {
                      lp_myStepper.step(-1);
                      //delay(20);
                      IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                      IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                    }
              }

  
              Serial.println("ACCLIMATION > START ");

              // TIME VARIABLE FOR ACCLIMATION
              unsigned long interval_acclimation = ACCLIMATION_LENGHT_MIN*60*1000L;
              unsigned long start_acclimation = millis();
              unsigned long current_acclimation = millis();
              
              // KEEP ARDUINO IN A LOOP TO SIMULATE THE DELAY WITHOUT MESSING WITH TIME (EG USING DELAY)
              while (current_acclimation - start_acclimation < interval_acclimation) {
                current_acclimation = millis();
              }

              // SET THE INITIAL VARIABLE INTERVAL AND VARIABLE RATIO TO 1
              CURRENT_VI = 1;
              CURRENT_VR = 1;

              // SET INITIAL SESSION START
              trial_start_time = millis();
              trial_current_time = millis();
  
              TRIAL_START = false;                     
              START = millis();                        // VARIABLE USED TO CALCULATE LP/MIN DURING TRIAL
              Serial.println("ACCLIMATION > END");
            }
  
  
            // ENTER COOLDOWNSTOP AND STOP TRIAL AFTER TRIAL LENGHT
            /*##################################################################################*/
            if (trial_current_time - trial_start_time < (TRIAL_LENGHT_MIN * 60 * 1000L)) {
              // DO NOTHING
              trial_current_time = millis();
            } else {
              
              SESSION_CURRENT_STATUS == 0;
  
              // ENSURE THAT LEVER IS RETRACTED DURING COOLDOWN
              /*---------------------------------------------------------*/
              IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
              IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
              if (IS_LEVER_OUTSIDE == 1) { // IF LEVER IS OUTSIDE move IT INSIDE
                    while (IS_LEVER_INSIDE != 1) {
                      lp_myStepper.step(-1);
                      //delay(20);
                      IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                      IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                    }
              }
              
              // ENTER COOLDOWN
              Serial.println("COOLDOWN > START");
              delay(COOLDOWN_LENGHT_MIN*60*1000L);
              Serial.println("COOLDOWN > END");
              
              // STOP TRIAL AND PRINT TRIAL STATISTICS
              /*---------------------------------------------------------*/
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" > END");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" STATISTICS");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP/MIN: "); Serial.println(CUMULATIVE_SUM_LP/TRIAL_LENGHT_MIN);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL TOTAL LP: "); Serial.println(CUMULATIVE_SUM_LP);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL TOTAL N PELLETS: "); Serial.println(N_PELLETS);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP BEFORE CS+: ");
                for(int i = 0; i < TOTAL_CS_NUMBER; i++) { 
                  Serial.print(lp_pre_cs_list[i]); Serial.print(","); }; Serial.println("");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP DURING CS+: ");
                for(int i = 0; i < TOTAL_CS_NUMBER; i++) {
                  Serial.print(lp_peri_cs_list[i]); Serial.print(","); }; Serial.println("");


              // SAVE TRIAL MEASUREMENTS IN GLOBAL SESSION VARIABLES
              TOTAL_SESSION_LP = TOTAL_SESSION_LP + CUMULATIVE_SUM_LP;
              TOTAL_SESSION_PELLETS = TOTAL_SESSION_PELLETS + N_PELLETS;

              // RESET TRIAL LP STATISTICS VARIABLES
              CUMULATIVE_SUM_LP = 0;
              LP_MIN = 0;
              LP = 0;
              LP_MINS = 0;
              LP_AVG= 0;
              N_PELLETS = 0;
              
              break;
         
            }
            
            // ALLOW ACCESS TO THE LEVER DURING TRIAL
            /*##################################################################################*/
            IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
            IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
  
            if (IS_LEVER_INSIDE == 1) {
              while (IS_LEVER_OUTSIDE != 1) {
                lp_myStepper.step(1);
                IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
              }
            }
            
  
            // CALCULATE LEVER PRESSES PER MINUTE
            /*##################################################################################*/
            unsigned long CURRENT = millis();
            if (CURRENT - START >= INTERVAL) {
  
              // ADD ONE MINUTE TO COUNTER
              LP_MINS ++;
  
              // RE-ASSIGN START MILLIS
              START = millis();
  
              // PRINT LP MEASUREMENTS
              LP_AVG = LP / LP_MINS;
              Serial.print("GLOBAL TRIAL LP/MIN: "); Serial.println(LP_AVG);
              Serial.print("LP/MIN - PREVIOUS MINUTE: "); Serial.println(LP_MIN);
              Serial.print("CUMULATIVE TRIAL LP: "); Serial.println(CUMULATIVE_SUM_LP);
              Serial.print("CUMULATIVE TRIAL N PELLETS: "); Serial.println(N_PELLETS);
              Serial.print("ELAPSED TRIAL TIME (SEC): "); Serial.println((trial_current_time - trial_start_time)/(1000L));
              
              LP_MIN = 0;             // RESET VARIABLE
            }
  
  
            // CODE TO DELIVER CS-US
            /*##################################################################################*/   
            CS_ONGOING_TIME = millis();
            unsigned long iti_delay_ongoing_time = millis();
                 
            // AFTER TWO MINUTES POST-HABITUTATION
            
            if (trial_current_time - trial_start_time > (LENGHT_WITH_LEVER_BEFORE_FIRST_CS * 60 * 1000L)) { 
  
              // IF THERE ARE MISSING CS
              if (CS_NUMBER <= TOTAL_CS_NUMBER) {
  
                // DELIVER CS
                //###################################################################################        
                if ((DELIVER_CS == true) && (IS_INTER_CS_DELAY == false)) {

                  // PRINT NUMBER OF LEVER PRESS BEFORE CS
                  /*-----------------------------------------------------------------------------------------------------------------------------------------*/
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" LP BEFORE CS+ 0"); Serial.print(CS_NUMBER);
                    Serial.print(": "); Serial.println(CUMULATIVE_SUM_LP - pre_cs_lp_start); 
                  
                  lp_pre_cs_list[list_end] = CUMULATIVE_SUM_LP - pre_cs_lp_start;
                  
                  Serial.print("TRIAL 0"); Serial.print(i);Serial.print(" CS+ 0"); Serial.print(CS_NUMBER); Serial.println(" > ON");
                  digitalWrite(CS_PIN, HIGH);
                  digitalWrite(CS_LED_PIN, HIGH);

                  // TRACK THE BEGUINNING OF THE CS
                  CS_START_TIME = millis();
                  DELIVER_CS = false;
                  DELIVER_US = true;
  
                  // KEEP TRACK OF THE LP DURING CS
                  TOTAL_LP_BEFORE_CS = CUMULATIVE_SUM_LP;
                  
                } else if ((DELIVER_CS == false)
                           && ((CS_ONGOING_TIME - CS_START_TIME) > ((CS_LENGHT_SEC - US_PAIRED_LENGTH_SEC) * 1000L)) 
                           && (IS_INTER_CS_DELAY == false)
                           && (DELIVER_US == true)){

                 if (TRIAL_WITH_US == true) {                
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" US"); Serial.println(" > ON");
                  digitalWrite(US_PIN, HIGH);
                 }
                 DELIVER_US = false;
                   
                } else if ((DELIVER_CS == false)
                           && ((CS_ONGOING_TIME - CS_START_TIME) > (CS_LENGHT_SEC * 1000L)) 
                           && (IS_INTER_CS_DELAY == false)){
  
                  Serial.print("TRIAL 0"); Serial.print(i);Serial.print(" CS+ 0"); Serial.print(CS_NUMBER); Serial.println(" > OFF");
                  digitalWrite(CS_PIN, LOW);
                  digitalWrite(CS_LED_PIN, LOW);

                  if (TRIAL_WITH_US == true) {
                    Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" US"); Serial.println(" > OFF");
                    digitalWrite(US_PIN, LOW);
                  }
  
                  // KEEP TRACK OF THE LP DURING CS
                  TOTAL_LP_DURING_CS = CUMULATIVE_SUM_LP - TOTAL_LP_BEFORE_CS;
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" LP DURING CS+ 0"); Serial.print(CS_NUMBER);
                    Serial.print(": "); Serial.println(TOTAL_LP_DURING_CS);

                  // STORE VALUE 
                  lp_peri_cs_list[list_end++] = TOTAL_LP_DURING_CS;
                                   
                  // MOVE TO THE NEXT CS
                  CS_NUMBER++;                  
                  DELIVER_CS = true;

                  // ALLOW DELIVERY OF A PELLET 60 SECONDS AFTER ITI START
                  ITI_PELLETS = true;
  
                  // PRINT INTER-CS-INTERVAL 
                  int delay_iti = INTER_TRIAL_INTERVAL_LIST[random(0, 3)];
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" INTER-CS-INTERVAL (SEC): "); Serial.println(delay_iti);
                  INTER_CS_DELAY_LENGTH = delay_iti * 1000L;
                  INTER_CS_DELAY_START = millis();
                  IS_INTER_CS_DELAY = true;

                  // ALLOW ACCESS TO THE LEVER
                  IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                  IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
        
                  if (IS_LEVER_INSIDE == 1) {
                    while (IS_LEVER_OUTSIDE != 1) {
                      lp_myStepper.step(1);
                      IS_LEVER_OUTSIDE = digitalRead(LEVER_OUT_DETECTOR_PIN);
                      IS_LEVER_INSIDE = digitalRead(LEVER_IN_DETECTOR_PIN);
                    }
                  }
  
                } else if ((DELIVER_CS == true) && (IS_INTER_CS_DELAY == true)){      // ACTIVE DURING ITI

                  // CONDITION TO TERMINATE ITI
                  if ((iti_delay_ongoing_time - INTER_CS_DELAY_START) > INTER_CS_DELAY_LENGTH) {
                    IS_INTER_CS_DELAY = false;    


                  // KEEP TRACK OF THE LP IN THE 60 SECONDS BEFORE CS
                  } else if ((iti_delay_ongoing_time - INTER_CS_DELAY_START) <= (INTER_CS_DELAY_LENGTH - 60*1000L)) {
                    pre_cs_lp_start = CUMULATIVE_SUM_LP;
                  }


                  // ACTIVE FEEDER FOR A PELLET 60 AFTER ITI STARTS
                  if (((iti_delay_ongoing_time - INTER_CS_DELAY_START) > 60000) && (ITI_PELLETS == true)) {

                      // DROP A PELLET
                      /*-----------------------------------*/
                      myStepper.step(FEEDER_STEPS_PER_REVOLUTION/4);
                      Serial.println("FEEDER > ON");
                      N_PELLETS ++;

                      // PELLET OFF
                      ITI_PELLETS = false;
                     
                  }
                  
                }
                
              }
              
            } else {
              // LENGHT_WITH_LEVER_BEFORE_FIRST_CS
              // KEEP IN TRACK HOW MANY LP ARE GIVEN IN THE 60 SECONDS BEFORE 1ST CS
              if ((trial_current_time - trial_start_time < ((LENGHT_WITH_LEVER_BEFORE_FIRST_CS - 1) * 60 * 1000L))) {
                pre_cs_lp_start = CUMULATIVE_SUM_LP;
              }
              
            }
  
            // GET CURRENT TIME
            unsigned long current_time = millis();
  
            // CODE TO CONTROL FEEDER AND LEVER PRESS
            /*##################################################################################*/
  
            // CHECK IF VI IS OVER
            if (current_time - previous_time <= CURRENT_VI) {
              
              // IF NO THEN
              COUNT_TOWARDS_VR = false;                                // ACCESS TO VR == NUMBER OF LP NECESSARY TO ACTIVATE THE MAGAZINE
  
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
              
              // DELIVER PELLET (REVOLUTIONS MUST BE ADJUSTED TO ENSURE 1 PELLET AT TIME)
              myStepper.step(FEEDER_STEPS_PER_REVOLUTION/4);
              Serial.println("FEEDER > ON");
              N_PELLETS ++;
              
              // RESET COUNTING PRESSES FOR VR
              LP_TOWARDS_CURRENT_VR_THRESHOLD = 0;
  
              // RESET THE VR FOR NEXT TRIAL
              CURRENT_VR = random(1, MAX_VR_LP+1);
              Serial.print("NEW VR (LP): "); Serial.println(CURRENT_VR);
  
              // RE-INITIATE VI
              COUNT_TOWARDS_VR = false;                    // DETERMINES ACCESS TO FEEDER
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
  
    }
    // IF TRIAL IS THE LAST, PRINT STATS FOR THE WHOLE SESSION
    if (i == N_TRIALS) {
      
      Serial.println("SESSION > END");
      Serial.print("SESSION FINAL LP/MIN: "); Serial.println(TOTAL_SESSION_LP/((N_TRIALS*TRIAL_LENGHT_MIN)));
      Serial.print("SESSION TOTAL N LP: "); Serial.println(TOTAL_SESSION_LP);
      Serial.print("SESSION TOTAL N PELLETS: "); Serial.println(TOTAL_SESSION_PELLETS);

      // ENTER ENDLESS LOOP > FORCE USER TO RESET ARDUINO
      while (true) {
        // DO NOTHING
      }
      
    }

  }
  } else {
    // do nothing
      }
}
