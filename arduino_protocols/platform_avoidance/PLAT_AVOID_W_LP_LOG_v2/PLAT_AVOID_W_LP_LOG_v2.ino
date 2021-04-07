
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu

PLAT_AVOID_W_LP_LOG_v2
------------------------
Log lever presses and deliver food pellets when due.


*/

// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>

// SESSION AND TRIAL INFO >> TO MODIFY
/*##################################################################################*/
int N_TRIALS = 1;                                       // NUMBER OF TRIALS
unsigned long TRIAL_LENGHT_MIN = 13;                    
unsigned long ACCLIMATION_LENGHT_MIN = 0;                  
unsigned long COOLDOWN_LENGHT_MIN = 2;                     
unsigned long LENGH_WITH_LEVER_BEFORE_FIRST_CS = 0;     // LENGTH AFTER ACCLIMATION...
              
// CS AND US VARIABLES >> TO MODIFY
/*##################################################################################*/
const int TOTAL_CS_NUMBER = 3;    // NUMBER OF CS
int CS_LENGHT_SEC = 30;           // SECONDs
bool TRIAL_WITH_US = true;        // IF FALSE THEN NO US
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


// VI AND VR - STARTING VALUE (FOR FIRST TRIAL)
/*##################################################################################*/
int variable_ratio = 1;                                      // STARTING VALUE FOR VR
unsigned long variable_interval = 1 * 1000L;                 // STARTING VALUE FOR VI

// CONTROL TRANSITION BETWEEN VI AND VR
/*##################################################################################*/
unsigned long previous_time = millis();
bool access = false;                             // CONTROL THE TRANSITION TO VR


// TO KEEP TRACK OF LP/MIN
/*##################################################################################*/
unsigned long START;
unsigned long INTERVAL = 60*1000L;


// KEEP TRACK OF MEASUREMENTS AND ALLOW BASIC LP STATS
/*##################################################################################*/
int LP_MIN = 0;
int LP = 0;
int LP_MINS = 0;
int LP_AVG= 0;
int N_PELLETS = 0;
bool ITI_PELLETS;

int net_cs_lp;
int start_lp;


// TRIAL INFORMATION
/*##################################################################################*/
bool TRIAL_START = true; // acts as boolean
int session_status = 1;              // WHEN ZERO THEN SESSION IS OVER


// VARIABLES TO DELIVER CS
/*##################################################################################*/
bool cs_ongoing = false;
int itintervals[] = {120, 120, 120};
bool cs_ready = true;
bool iti_delay = false;
unsigned long cs_start_time;
unsigned long iti_delay_start_time;
unsigned long iti_delay_length;                       // SECONDS
bool us_ready;

// SETTINGS FOR BOARD AND STEPPER - DO NOT MODIFY
/*##################################################################################*/
const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 8;         // LED in the chamber food tray
const int lever_press = 9;           // LEVER_PRESS DECTECTOR


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

// INITIALIZE STEPPER:
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

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

  // TO DELIVER CS
  pinMode(4, OUTPUT);  // DELIVER CS
  pinMode(5, OUTPUT);  // CONTROL LED

  // TO DELIVER US
  pinMode(6, OUTPUT);  // DElIVER US

  // ENSURE REPRODUCIBILITY
  int seed = 31;
  randomSeed(seed);

}

void loop() {

  // START TRIAL
  int button_state = 0;

  // TEST FEEDER
  /*##################################################################################*/ 
  int push_button_test_feeder_state = digitalRead(push_button_test_feeder);
  
  if (push_button_test_feeder_state > 0) {
    myStepper.step(stepsPerRevolution/4);
    
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
   

  
  if (button_state == HIGH) {

    // KEEP TRACK OF GLOBAL VARIABLES
    int TOTAL_SESSION_LP = 0;
    int TOTAL_SESSION_PELLETS = 0;

    
    // DISPLAY SESSION INFORMATION
    /*##################################################################################*/
    Serial.println("SESSION: PLATFORM AVOIDANCE WITH LEVER PRESS LOG");
    Serial.print("TRIAL LENGHT (SEC): "); Serial.println((TRIAL_LENGHT_MIN * 60));
    Serial.print("NUMBER OF TRIALS: "); Serial.println(N_TRIALS); 
    Serial.print("NUMBER OF CS+: "); Serial.println(TOTAL_CS_NUMBER);
    Serial.print("CS+ DURATION (SEC): "); Serial.println(CS_LENGHT_SEC);
    if (TRIAL_WITH_US == true) {
      Serial.print("NUMBER OF US: "); Serial.println(TOTAL_CS_NUMBER);
    }
    Serial.print("STARTING VI (SEC): "); Serial.print(variable_interval / 1000); Serial.print(" | STARTING VR: "); Serial.println(variable_ratio);  
    Serial.print("SESSION VI (SEC): "); Serial.print(MAX_VI_SEC); Serial.print(" | SESSION VR: "); Serial.println(MAX_VR_LP);  
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(ACCLIMATION_LENGHT_MIN*60);
    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(COOLDOWN_LENGHT_MIN*60);
    Serial.println("SESSION > START");


    // INITIATE TRIALS
    /*##################################################################################*/
    for (int i = 1; i < (N_TRIALS + 1); i++) {


      // PRINT TRIAL INFORMATION
      Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" > START");
      
      session_status = 1;                            // IF==1 > SESSION IS "ON"
      
      // TRIAL TIME VARIABLES
      unsigned long trial_start_time;
      unsigned long trial_current_time;

      TRIAL_START = true;                            // FALSE==END  
      
      int current_cs_plus = 1;

      // TRACK LP PRE CS
      int pre_cs_lp_start = 0;
    
      // KEEP TRACK OF THE LP PRE-CS AND PERI-CS
      int lp_pre_cs_list[TOTAL_CS_NUMBER];
      int lp_peri_cs_list[TOTAL_CS_NUMBER];
      byte list_end = 0 ; // the end pointer

      

      // LOOP UNTIL THE TRIAL IS OVER (==TIME LIMIT)
      /*##################################################################################*/
      while (session_status == 1) {
  
            // TEST FEEDER
            /*##################################################################################*/ 
            int push_button_test_feeder_state = digitalRead(push_button_test_feeder);
            
            if (push_button_test_feeder_state > 0) {
              myStepper.step(stepsPerRevolution/4);
              Serial.println("FEEDER > ON");
              // ADD SMALL DELAY
              unsigned long start_ = millis();
              unsigned long delay_ = millis();
              while (delay_ - start_ < 1000) {
                // DO NOTHING
                delay_ = millis(); 
            }
              
            }
  
            // ACCLIMATION PERIOD > LEVER IS RETRACTED
            /*##################################################################################*/
            if (TRIAL_START == true) {
  
              // ENSURE THAT LEVER IS RETRACTED DURING ACCLIMATION
              /*---------------------------------------------------------*/
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
              variable_interval = 1;
              variable_ratio = 1;

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
              
              session_status == 0;
  
              // ENSURE THAT LEVER IS RETRACTED DURING COOLDOWN
              /*---------------------------------------------------------*/
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
              Serial.println("COOLDOWN > START");
              delay(COOLDOWN_LENGHT_MIN*60*1000L);
              Serial.println("COOLDOWN > END");
              
              // STOP TRIAL AND PRINT TRIAL STATISTICS
              /*---------------------------------------------------------*/
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" > END");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.println(" STATISTICS");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP/MIN: "); Serial.println(cumsum_presses/TRIAL_LENGHT_MIN);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL TOTAL LP: "); Serial.println(cumsum_presses);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL TOTAL N PELLETS: "); Serial.println(N_PELLETS);
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP BEFORE CS+: ");
                for(int i = 0; i < TOTAL_CS_NUMBER; i++) { 
                  Serial.print(lp_pre_cs_list[i]); Serial.print(","); }; Serial.println("");
              Serial.print("TRIAL N 0"); Serial.print(i); Serial.print(" FINAL LP DURING CS+: ");
                for(int i = 0; i < TOTAL_CS_NUMBER; i++) {
                  Serial.print(lp_peri_cs_list[i]); Serial.print(","); }; Serial.println("");


              // SAVE TRIAL MEASUREMENTS IN GLOBAL SESSION VARIABLES
              TOTAL_SESSION_LP = TOTAL_SESSION_LP + cumsum_presses;
              TOTAL_SESSION_PELLETS = TOTAL_SESSION_PELLETS + N_PELLETS;

              // RESET TRAIL LP STATISTICS VARIABLES
              cumsum_presses = 0;
              LP_MIN = 0;
              LP = 0;
              LP_MINS = 0;
              LP_AVG= 0;
              N_PELLETS = 0;
              
              break;
         
            }
            
            // ALLOW ACCESS TO THE LEVER DURING TRIAL
            /*##################################################################################*/
            lp_inside = digitalRead(lp_inside_pin);
            lp_outside = digitalRead(lp_outside_pin);
  
            if (lp_inside == 1) {
              while (lp_outside != 1) {
                lp_myStepper.step(1);
                lp_outside = digitalRead(lp_outside_pin);
                lp_inside = digitalRead(lp_inside_pin);
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
              Serial.print("CUMULATIVE TRIAL LP: "); Serial.println(cumsum_presses);
              Serial.print("CUMULATIVE TRIAL N PELLETS: "); Serial.println(N_PELLETS);
              Serial.print("ELAPSED TRIAL TIME (SEC): "); Serial.println((trial_current_time - trial_start_time)/(1000L));
              
              LP_MIN = 0;             // RESET VARIABLE
            }
  
  
            // CODE TO DELIVER CS-US
            /*##################################################################################*/   
            unsigned long cs_current_time = millis();
            unsigned long iti_delay_ongoing_time = millis();
                 
            // AFTER TWO MINUTES POST-HABITUTATION
            
            if (trial_current_time - trial_start_time > (LENGH_WITH_LEVER_BEFORE_FIRST_CS * 60 * 1000L)) { 
  
              // IF THERE ARE MISSING CS
              if (current_cs_plus <= TOTAL_CS_NUMBER) {
  
                // DELIVER CS
                //###################################################################################        
                if ((cs_ready == true) && (iti_delay == false)) {

                  // PRINT NUMBER OF LEVER PRESS BEFORE CS
                  /*-----------------------------------------------------------------------------------------------------------------------------------------*/
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" LP BEFORE CS 0"); Serial.print(current_cs_plus);
                    Serial.print(": "); Serial.println(cumsum_presses - pre_cs_lp_start); 
                  
                  lp_pre_cs_list[list_end] = cumsum_presses - pre_cs_lp_start;
                  
                  Serial.print("TRIAL 0"); Serial.print(i);Serial.print(" CS+ 0"); Serial.print(current_cs_plus); Serial.println(" > ON");
                  digitalWrite(4, HIGH);
                  digitalWrite(5, HIGH);

                  // TRACK THE BEGUINNING OF THE CS
                  cs_start_time = millis();
                  cs_ready = false;
                  us_ready = true;
  
                  // KEEP TRACK OF THE LP DURING CS
                  start_lp = cumsum_presses;
                  
                } else if ((cs_ready == false)
                           && ((cs_current_time - cs_start_time) > ((CS_LENGHT_SEC - US_PAIRED_LENGTH_SEC) * 1000L)) 
                           && (iti_delay == false)
                           && (us_ready == true)){

                 if (TRIAL_WITH_US == true) {                
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" US"); Serial.println(" > ON");
                  digitalWrite(6, HIGH);
                 }
                 us_ready = false;
                   
                } else if ((cs_ready == false)
                           && ((cs_current_time - cs_start_time) > (CS_LENGHT_SEC * 1000L)) 
                           && (iti_delay == false)){
  
                  Serial.print("TRIAL 0"); Serial.print(i);Serial.print(" CS+ 0"); Serial.print(current_cs_plus); Serial.println(" > OFF");
                  digitalWrite(4, LOW);
                  digitalWrite(5, LOW);

                  if (TRIAL_WITH_US == true) {
                    Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" US"); Serial.println(" > OFF");
                    digitalWrite(6, LOW);
                  }
  
                  // KEEP TRACK OF THE LP DURING CS
                  net_cs_lp = cumsum_presses - start_lp;
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" LP DURING CS+ 0"); Serial.print(current_cs_plus);
                    Serial.print(": "); Serial.println(net_cs_lp);

                  // STORE VALUE 
                  lp_peri_cs_list[list_end++] = net_cs_lp;
                  
                  
                  // MOVE TO THE NEXT CS
                  current_cs_plus++;                  
                  cs_ready = true;


                  // ALLOW DELIVERY OF A PELLET 60 SECONDS AFTER ITI START
                  ITI_PELLETS = true;
  
                  // PRINT INTER-CS-INTERVAL 
                  int delay_iti = itintervals[random(0, 3)];
                  Serial.print("TRIAL 0"); Serial.print(i); Serial.print(" INTER-CS-INTERVAL (SEC): "); Serial.println(delay_iti);
                  iti_delay_length = delay_iti * 1000L;
                  iti_delay_start_time = millis();
                  iti_delay = true;

                  // ALLOW ACCESS TO THE LEVER
                  lp_inside = digitalRead(lp_inside_pin);
                  lp_outside = digitalRead(lp_outside_pin);
        
                  if (lp_inside == 1) {
                    while (lp_outside != 1) {
                      lp_myStepper.step(1);
                      lp_outside = digitalRead(lp_outside_pin);
                      lp_inside = digitalRead(lp_inside_pin);
                    }
                  }
  
                  
                } else if ((cs_ready == true) && (iti_delay == true)){      // ACTIVE DURING ITI

                  // CONDITION TO TERMINATE ITI
                  if ((iti_delay_ongoing_time - iti_delay_start_time) > iti_delay_length) {
                    iti_delay = false;    


                  // KEEP TRACK OF THE LP IN THE 60 SECONDS BEFORE CS
                  } else if ((iti_delay_ongoing_time - iti_delay_start_time) <= (iti_delay_length - 60*1000L)) {
                    pre_cs_lp_start = cumsum_presses;
                  }


                  // ACTIVE FEEDER FOR A PELLET 60 AFTER ITI STARTS
                  if (((iti_delay_ongoing_time - iti_delay_start_time) > 60000) && (ITI_PELLETS == true)) {

                      // DROP A PELLET
                      /*-----------------------------------*/
                      myStepper.step(stepsPerRevolution/4);
                      Serial.println("FEEDER > ON");
                      N_PELLETS ++;

                      // PELLET OFF
                      ITI_PELLETS = false;
                     
                  }
                  
                }
                
              }
              
            } else {
              // LENGH_WITH_LEVER_BEFORE_FIRST_CS
              // KEEP IN TRACK HOW MANY LP ARE GIVEN IN THE INITIAL 60 SECONDS BEFORE 1ST CS
              if ((trial_current_time - trial_start_time < (LENGH_WITH_LEVER_BEFORE_FIRST_CS))) {
                pre_cs_lp_start = cumsum_presses;
              }
              
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
              
              // DELIVER PELLET (REVOLUTIONS MUST BE ADJUSTED TO ENSURE 1 PELLET AT TIME)
              myStepper.step(stepsPerRevolution/4);
              Serial.println("FEEDER > ON");
              N_PELLETS ++;
              
              // RESET COUNTING PRESSES FOR VR
              counting_presses = 0;
  
              // RESET THE VR FOR NEXT TRIAL
              variable_ratio = random(1, MAX_VR_LP+1);
              Serial.print("NEW VR (LP): "); Serial.println(variable_ratio);
  
              // RE-INITIATE VI
              access = false;                    // DETERMINES ACCESS TO FEEDER
              previous_time = millis();
              variable_interval = random(1, MAX_VI_SEC+1) * 1000L;
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
  
    }
    // IF TRIAL IS THE LAST, PRINT STATS FOR THE WHOLE SESSION
    if (i == N_TRIALS) {
      
      Serial.println("SESSION > END");
      Serial.print("SESSION FINAL LP/MIN: "); Serial.println(TOTAL_SESSION_LP/((N_TRIALS*TRIAL_LENGHT_MIN)));
      Serial.print("SESSION TOTAL N LP: "); Serial.println(TOTAL_SESSION_LP);
      Serial.print("SESSION TOTAL N PELLETS: "); Serial.println(TOTAL_SESSION_PELLETS);
      
    }

  }
  } else {
    // do nothing
      }
}
