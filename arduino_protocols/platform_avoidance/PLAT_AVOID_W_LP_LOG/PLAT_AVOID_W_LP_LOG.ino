
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu

PLAT_AVOID_W_LP_LOG
------------------------
Log lever presses and deliver food pellets when due.


*/

// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>

// SESSION AND TRIAL INFO
/*##################################################################################*/
unsigned long trial_length = 12 * 60 * 1000L;                // DURATION OF THE SESSION >> "MIN * SEC * MS"
int N_TRIALS = 1;
unsigned long acclimation_length = 1;                        // DURATION IN MIN
unsigned long cooldown_length = 1;                           // DURATION IN MIN
unsigned long post_habituation_delay = 180000;               // NO CS, JUST LEVER PRESS

// CS VARIABLES
/*##################################################################################*/
const int cs_total = 3;                                // NUMBER OF CS
int cs_duration = 30;                                  // SECONDs

// US VARIABLES
/*##################################################################################*/
bool us_on = false;                                     // IF FALSE THEN NO US
int us_start = 2;                                       // 

// VI AND VR
/*##################################################################################*/
int max_vr = 4;                                   // MAX VARIABLE RATIO FOR RANDOM GENERATOR
int max_vi = 30;                                  // MAX VARIABLE INTERVAL FOR RANDOM GENERATOR

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
  Serial.print("LEVER PRESS LOG"); Serial.print(" | VI"); Serial.print(max_vi); Serial.print(" | VR0"); Serial.println(max_vr);

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
    Serial.print("TRIAL LENGHT (SEC): "); Serial.println(trial_length / (1000L));
    Serial.print("NUMBER OF TRIALS: "); Serial.println(N_TRIALS); 
    Serial.print("NUMBER OF CS-PLUS: "); Serial.println(cs_total);
    Serial.print("CS-PLUS DURATION (SEC): "); Serial.println(cs_duration);
    if (us_on == true) {
      Serial.print("NUMBER OF US: "); Serial.println(cs_total);
    }
    Serial.print("STARTING VI (SEC): "); Serial.print(variable_interval / 1000); Serial.print(" | STARTING VR: "); Serial.println(variable_ratio);  
    Serial.print("SESSION VI (SEC): "); Serial.print(max_vi); Serial.print(" | SESSION VR: "); Serial.println(max_vr);  
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(acclimation_length*60);
    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(cooldown_length*60);
    Serial.println("SESSION: START");


    // INITIATE TRIALS
    /*##################################################################################*/
    for (int i = 1; i < (N_TRIALS + 1); i++) {

      Serial.print("TRIAL N: 0"); Serial.println(i);
      Serial.println("TRIAL: START");
      session_status = 1;
      unsigned long trial_start_time;
      unsigned long trial_current_time;
      TRIAL_START = true;
      int current_cs_plus = 1;

      int pre_cs_lp_start;
      int lp_pre_cs_list[cs_total];
      int lp_peri_cs_list[cs_total];
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

              // SET THE INITIAL VARIABLE INTERVAL AND VARIABLE RATIO TO 1
              variable_interval = 1;
              variable_ratio = 1;

              // SET INITIAL SESSION START
              trial_start_time = millis();
              trial_current_time = millis();
  
              TRIAL_START = false;
              START = millis();                        // VARIABLE USED TO CALCULATE LP/MIN DURING TRIAL
              Serial.println("ACCLIMATION: OFF");
            }
  
  
            // ENTER COOLDOWNSTOP AND STOP TRIAL AFTER TIME LIMIT
            /*##################################################################################*/
            if (trial_current_time - trial_start_time < trial_length) {
              // DO NOTHING
              trial_current_time = millis();
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
              
              // STOP TRIAL
              Serial.println("TRIAL: END");
              Serial.print("TRIAL N: 0"); Serial.print(i); Serial.println(" STATS");
              Serial.print("GLOBAL TRIAL LP/MIN: "); Serial.println(cumsum_presses/(trial_length/60000L));
              Serial.print("CUMULATIVE TRIAL LP: "); Serial.println(cumsum_presses);
              Serial.print("CUMULATIVE TRIAL N PELLETS: "); Serial.println(N_PELLETS);
              Serial.print("LP BEFORE CS: "); for(int i = 0; i < cs_total; i++) { Serial.print(lp_pre_cs_list[i]); Serial.print(" | "); }; Serial.println("");
              Serial.print("LP DURING CS: "); for(int i = 0; i < cs_total; i++) { Serial.print(lp_peri_cs_list[i]); Serial.print(" | "); }; Serial.println("");


              // SAVE TRIAL MEASUREMENTS IN GLOBAL SESSION VARIABLES
              TOTAL_SESSION_LP = TOTAL_SESSION_LP + cumsum_presses;
              TOTAL_SESSION_PELLETS = TOTAL_SESSION_PELLETS + N_PELLETS;

              // RESET LP STATISTICS VARIABLES
              cumsum_presses = 0;
              LP_MIN = 0;
              LP = 0;
              LP_MINS = 0;
              LP_AVG= 0;
              N_PELLETS = 0;
              
              break;
         
            }
            
            // ALLOW ACCESS TO THE LEVER
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
              LP_MIN = 0;
            }
  
  
            // CODE TO DELIVER CS-US
            /*##################################################################################*/   
            unsigned long cs_current_time = millis();
            unsigned long iti_delay_ongoing_time = millis();
                 
            // AFTER TWO MINUTES POST-HABITUTATION
            
            if (trial_current_time - trial_start_time > post_habituation_delay) { 
  
              // IF THERE ARE MISSING CS
              if (current_cs_plus <= cs_total) {
  
                // DELIVER CS
                //###################################################################################        
                if ((cs_ready == true) && (iti_delay == false)) {
  
                  Serial.print("LP BEFORE CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.print(" > "); Serial.println(cumsum_presses - pre_cs_lp_start); 
                  lp_pre_cs_list[list_end] = cumsum_presses - pre_cs_lp_start;
                  
                  Serial.print("CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.println(" > ON");
                  digitalWrite(4, HIGH);
                  digitalWrite(5, HIGH);
                  cs_start_time = millis();
                  cs_ready = false;
                  us_ready = true;
  
                  // KEEP TRACK OF THE LP DURING CS
                  start_lp = cumsum_presses;
                  
                } else if ((cs_ready == false)
                           && ((cs_current_time - cs_start_time) > ((cs_duration - us_start) * 1000L)) 
                           && (iti_delay == false)
                           && (us_ready == true)){

                 if (us_on == true) {                
                  Serial.print("US"); Serial.println(" > ON");
                  digitalWrite(6, HIGH);
                 }
                 us_ready = false;
                   
                } else if ((cs_ready == false)
                           && ((cs_current_time - cs_start_time) > (cs_duration * 1000L)) 
                           && (iti_delay == false)){
  
                  Serial.print("CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.println(" > OFF");
                  digitalWrite(4, LOW);
                  digitalWrite(5, LOW);

                  if (us_on == true) {
                    Serial.println("US > OFF");
                    digitalWrite(6, LOW);
                  }
  
                  // KEEP TRACK OF THE LP DURING CS
                  net_cs_lp = cumsum_presses - start_lp;
                  Serial.print("LP DURING CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.print(" > "); Serial.println(net_cs_lp);
                  lp_peri_cs_list[list_end++] = net_cs_lp;
                  
                  
                  // MOVE TO THE NEXT CS
                  current_cs_plus++;
                  
                  cs_ready = true;


                  // ALLOW DELIVERY OF A PELLET 60 SECONDS AFTER ITI START
                  ITI_PELLETS = true;
  
                  // PRINT INTER-TRIAL-INTERVAL 
                  int delay_iti = itintervals[random(0, 3)];
                  Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(delay_iti);
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
  
                  
                } else if ((cs_ready == true) && (iti_delay == true)){
                  
                  if ((iti_delay_ongoing_time - iti_delay_start_time) > iti_delay_length) {
                    iti_delay = false;
                  } else if ((iti_delay_ongoing_time - iti_delay_start_time) <= (iti_delay_length - 30000)) {
                    pre_cs_lp_start = cumsum_presses;
                  }


                  // ACTIVE FEEDER FOR A PELLET 60 AFTER ITI STARTS
                  if (((iti_delay_ongoing_time - iti_delay_start_time) > 60000) && (ITI_PELLETS == true)) {

                      // DROP A PELLET
                      myStepper.step(stepsPerRevolution/4);
                      Serial.println("FEEDER > ON");
                      N_PELLETS ++;

                      // PELLET off
                      ITI_PELLETS = false;
                     
                  }
                  
                }
                
              }
              
            } else {
              
              // KEEP IN TRACK HOW MANY LP ARE GIVEN IN THE INITIAL 30 SECONDS BEFORE 1ST CS
              if ((trial_current_time - trial_start_time < ((acclimation_length*60*1000L) + 90000))) {
                pre_cs_lp_start = cumsum_presses;
              }
              
            }
  
            // GET CURRENT TIME
            unsigned long current_time = millis();
  
            // CODE TO CONTROL MAGAZINE AND LEVER PRESS
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
              variable_ratio = random(1, max_vr+1);
              Serial.print("NEW VR (LP): "); Serial.println(variable_ratio);
  
              // RE-INITIATE VI
              access = false;                    // DETERMINES ACCESS TO FEEDER
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
  
    }
    // IF TRIAL IS THE LAST, PRINT STATS FOR THE WHOLE SESSION
    if (i == N_TRIALS) {
      
      Serial.println("SESSION: END");
      Serial.print("GLOBAL SESSION LP/MIN: "); Serial.println(TOTAL_SESSION_LP/((N_TRIALS*trial_length)/60000L));
      Serial.print("CUMULATIVE SESSION LP: "); Serial.println(TOTAL_SESSION_LP);
      Serial.print("CUMULATIVE SESSION N PELLETS: "); Serial.println(TOTAL_SESSION_PELLETS);
      
    }

  }
  } else {
    // do nothing
      }
}
