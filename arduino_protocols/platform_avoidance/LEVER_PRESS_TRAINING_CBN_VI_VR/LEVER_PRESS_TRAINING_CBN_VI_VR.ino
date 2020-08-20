
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu

LEVER_PRESS_TRAINING_CBN
----------------------
Log lever presses and deliver food pellets when due according to a
pre determined variable interval and ratio. 


*/

// STEPPER LIBRARY FOR STEPPER CONTROL
#include <Stepper.h>

<<<<<<< HEAD

=======
>>>>>>> 3bc4cffb213521df1e0aeb07ad6422db6c70b0ff
// VI AND VR
unsigned long session_length = 15 * 60 * 1000L; // DURATION OF THE SESSION >> "MIN * SEC * MS"
int max_vr = 4;                                 // MAX VARIABLE RATIO FOR RANDOM GENERATOR
int max_vi = 30;                                 // MAX VARIABLE INTERVAL FOR RANDOM GENERATOR

<<<<<<< HEAD
unsigned long variable_interval = 1 * 1000L;    // STARTING VALUE FOR VI
int variable_ratio = 1;                         // STARTING VALUE FOR VR: MIN-MAX (EXCLUSIVE)


// ACCLIMATION / COOLDOWN

unsigned long acclimation_length = 0; // DURATION IN MIN 
unsigned long cooldown_length = 0;    // DURATION IN MIN 


=======

unsigned long variable_interval = 1 * 1000L;    // STARTING VALUE FOR VI
int variable_ratio = 1;                         // STARTING VALUE FOR VR: MIN-MAX (EXCLUSIVE)

>>>>>>> 3bc4cffb213521df1e0aeb07ad6422db6c70b0ff
// CONTROL TRANSITION BETWEEN VI AND VR
unsigned long previous_time = millis();
bool access = false;

// TO KEEP TRACK OF LP/MIN
unsigned long START = millis();
unsigned long INTERVAL = 60*1000L;

// BASIC LP STATS
int LP_MIN = 0;
int LP = 0;
int LP_MINS = 0;
int LP_AVG= 0;
int CUMSUM_LP = 0;


// TRIAL INFORMATION
int TRIAL_N = 50;    
bool TRIAL_START = true; // acts as boolean


// SETTINGS FOR BOARD AND STEPPER
const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 8;         // LED in the chamber food tray 
const int lever_press = 9;           // LEVER_PRESS DECTECTOR


// VARIABLES FOR LEVER PRESSING
int lever_state = 0;
int press_lapse = 0;
int counting_presses = 0;


// PUSH BUTTON TO START EXPERIMENT
const int push_button = 2;  


// INITIALIZE STEPPER:
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);


void setup() {

  // SET STEPPER SPEED
  myStepper.setSpeed(80); // STEPPER SPEED
  
  // INITIALIZE SERIAL
  Serial.begin(9600);
  
  // PRINT INITIAL INFORMATION
  Serial.println("----------------------------------");
  Serial.println("LEDOUX LAB");
  Serial.print("LEVER_PRESS_TRAINING"); 
  Serial.print(" | VI"); Serial.print(max_vi);
  Serial.print(" | VR0"); Serial.println(max_vr);
  Serial.println("----------------------------------");
  Serial.print("STARTING VI (SEC): "); Serial.print(variable_interval / 1000);
<<<<<<< HEAD
  Serial.print(" | STARTING VR: "); Serial.println(variable_ratio);
=======
  Serial.print("| STARTING VR: "); Serial.println(variable_ratio);
>>>>>>> 3bc4cffb213521df1e0aeb07ad6422db6c70b0ff
  Serial.println("PRESS GREEN BUTTON TO START");

  // SET UP PINS
  pinMode(food_tray_led, OUTPUT);
  pinMode(lever_press, INPUT);
  pinMode(push_button, INPUT);

  // ENSURE REPRODUCIBILITY 
  unsigned long seed = 31;
  randomSeed(seed);

}



void loop() {

  // START TRIAL 
  int button_state = 0;
  button_state = digitalRead(push_button);
  unsigned long session_start_time = millis();
  unsigned long session_current_time = millis();

  int session_status = 1; // Way to control the session loops 
  
  if (button_state == HIGH) {
    Serial.println("SESSION: START");
    
    while (session_status == 1) {

          // ACCLIMATION PERIOD
          if (TRIAL_START == true) {
                        
            Serial.print("ACCLIMATION (MIN): 0"); Serial.println(acclimation_length);
            unsigned long interval_acclimation = acclimation_length * 60 * 1000L;
            unsigned long start_acclimation = millis();
            unsigned long current_acclimation = millis();
        
            // KEEP ARDUINO IN A LOOP TO SIMULATE THE DELAY
            while (current_acclimation - start_acclimation < interval_acclimation) {
              current_acclimation = millis();
            }
            
            TRIAL_START = false;
            START = millis();
            Serial.println("ACCLIMATION: OFF");
          }
            
          /*#################################################*/
          
          // STOP SESSION AFTER SESSION LENGTH IS DUE
          if (session_current_time - session_start_time < session_length) {
            // DO NOTHING
            session_current_time = millis();
          } else {
            session_status == 0;
            // STOP SESSION
            Serial.print("COOLDOWN (MIN): 0"); Serial.println(cooldown_length);
            delay(cooldown_length * 60 * 1000L);
            Serial.println("SESSION: END");
            while(1){} // infinite loop until the  program is stopped by user
          }
            
           /*#################################################*/
          
        
          // CALCULATE LEVER PRESSES PER MINUTE
          unsigned long CURRENT = millis();
          if (CURRENT - START >= INTERVAL) {
        
            // ADD ONE MINUTE TO COUNTER
            LP_MINS ++;
        
            // RE-ASSIGN START MILLIS
            START = millis();
        
            // CALCULATE AVERAGE LP PER MIN GLOBALY
            LP_AVG = LP / LP_MINS;
            Serial.print("GLOBAL LP/MIN: "); Serial.println(LP_AVG);
            Serial.print("LP/MIN - PREVIOUS MINUTE: "); Serial.println(LP_MIN);
            Serial.print("CUMULATIVE LP: "); Serial.println(CUMSUM_LP);
            LP_MIN = 0;
          }
                
          // GET CURRENT TIME
          unsigned long current_time = millis();
          
          // CHECK IF VI IS OVER
          if (current_time - previous_time <= variable_interval) {
            // IF NO THEN
            access = false; // ACCESS TO VR
          } else {
            access = true; // ALLOW THE LEVER PRESSES TO COUNT TOWARDS VR
          }
        
          // ACCESS == TRUE > INITIATE VR
          if (access == true) {
        
          // COUNT LEVER PRESSES NECESSARY FOR THE APPROPRIATE VR
          if (counting_presses < variable_ratio){
        
                  // DETECT LEVER STATE
                  lever_state = digitalRead(lever_press); 
                  
                  // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                  if ((lever_state == HIGH) && (press_lapse == 0)) {
                    press_lapse = 1;
                    Serial.println("LEVER -> ON");
                    LP ++;
                    CUMSUM_LP ++;
                    LP_MIN ++;
                    delay(20);
                    
                    // KEEP TRACK OF THE NUMBER OF LEVER PRESSES
                    counting_presses ++;
                    
                  } else if ((lever_state == HIGH) && (press_lapse == 1)){
                    // DO NOTHING
                  } else if ((lever_state == LOW) && (press_lapse == 1)) {
                    press_lapse = 0;
                  } else {}
        
          } else {
            Serial.println("DELIVER FOOD");
        
            // MOVE HALF OF THE STEPPER REVOLUTIONS
            myStepper.step(stepsPerRevolution/2);
            
            // RESET COUNTING PRESSES FOR VR
            counting_presses = 0;
        
            // RESET THE VR
            variable_ratio = random(1, max_vr+1);
            Serial.print("NEXT VR: "); Serial.println(variable_ratio);
            
            // RE-INITIATE VI30
            access = false;
            previous_time = millis();
            variable_interval = random(1, max_vi+1) * 1000L;
            Serial.print("VI (SEC): "); Serial.println(variable_interval / 1000);
          } 
          
          } else { // LOG LEVER PRESSES DURING VI30
        
                    // DETECT LEVER STATE
                    lever_state = digitalRead(lever_press);
                    
                    // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
                    if ((lever_state == HIGH) && (press_lapse == 0)) {
                      press_lapse = 1;
                      Serial.println("LEVER -> ON");
                      LP ++;
                      CUMSUM_LP ++;
                      LP_MIN ++;
                      delay(20);
                  
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
