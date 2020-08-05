
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu

LEVER_PRESS_TRAINING_CBN
----------------------
This protocols is build to the subject to lever press in order to get the food in the Coulbourn Chambers. 


 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>



// VI30 AND VR04
unsigned long variable_interval = 1 * 1000L; // STARTING VALUE FOR VI
int variable_ratio = 4;                      // STARTING VALUE FOR VR

// CONTROL TRANSITION BETWEEN VI30 AND VR04
unsigned long previous_time = millis();
bool access = false;

// TO KEEP TRACK OF LP/MIN
unsigned long START = millis();
unsigned long INTERVAL = 60*1000L;
int LP_MIN = 0;

// TRIAL INFORMATION
int TRIAL_N = 50;    
int TRIAL_START = 1; // acts as bool


int track_lever = 0;
const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 8;         // LED in the chamber food tray 
const int lever_press = 9;           // LEVER_PRESS DECTECTOR

// VARIABLES FOR LEVER PRESSING
int lever_state = 0;
int press_lapse = 0;
int counting_presses = 0;


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

void setup() {

  // SET STEPPER SPEED
  myStepper.setSpeed(80); // Speed of the step.
  
  // INITIALIZE SERIAL
  Serial.begin(9600);
  
  // PRINT INITIAL INFORMATION
  Serial.println("LEVER_PRESS_TRAINING");
  Serial.println("VI30 | VR04");
  Serial.print("STARTING VI30 (ms): "); Serial.print(variable_interval);
  Serial.print("| STARTING VR04: "); Serial.println(variable_ratio);

  // SET UP PINs
  pinMode(food_tray_led, OUTPUT);
  pinMode(lever_press, INPUT);
  
}



void loop() {


  // ACCLIMATION PERIOD
  if (TRIAL_START == 1) {
    Serial.println("ACCLIMATION (min): 3");
    unsigned long interval_acclimation = 60*3*1000L;
    unsigned long start_acclimation = millis();
    unsigned long current_acclimation = millis();

    // KEEP ARDUINO IN A LOOP TO SIMULATE THE DELAY
    while (current_acclimation - start_acclimation < interval_acclimation) {
      current_acclimation = millis();
    }
    TRIAL_START = 0; // allow the normal code to be executed
    START = millis();
  }
  
  
  // COOLDOWN PERIOD IF TRIAL REACHES 0
  if (TRIAL_N == 0) {
    Serial.println("COOLDOWN (min): 3");
    delay(60*3*1000L);
    Serial.println("END");
    while(1){}                          // infinite loop until the  program is stopped by user
  }
  

  // CALCULATE LEVER PRESSES PER MINUTE
  unsigned long CURRENT = millis();
  if (CURRENT - START >= INTERVAL) {
    START = millis();
    Serial.print("LP/MIN: "); Serial.println(LP_MIN);
    LP_MIN = 0;
  }


  // GET CURRENT TIME
  unsigned long current_time = millis();

  // CHECK IF VI30 IS OVER
  if (current_time - previous_time <= variable_interval) {
    // IF NO THEN
    access = false; 
  } else {
    // ALLOW THE LEVER PRESSES TO COUNT TOWARDS VR04
    access = true;
  }

  // ACCESS == TRUE > INITIATE VR04
  if (access == true) {

  // COUNT LEVER PRESSES NECESSARY FOR THE APPROPRIATE VR04
  if (counting_presses < variable_ratio){

          // DETECT LEVER STATE
          lever_state = digitalRead(lever_press); 
          
          // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
          if ((lever_state == HIGH) && (press_lapse == 0)) {
            press_lapse = 1;
            Serial.println("LEVER -> ON");
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
    

    // RESET COUNTING PRESSES FOR VR04
    counting_presses = 0;

    // RESET THE VR04
    variable_ratio = random(1, 5);
    Serial.print("NEXT VR04: "); Serial.println(variable_ratio);
    
    // RE-INITIATE VI30
    access = false;
    previous_time = millis();
    variable_interval = random(1, 31) * 1000L;
    Serial.print("VI30 (ms): "); Serial.println(variable_interval);
  } }
  
  // LOG LEVER PRESSES DURING VI30
  else {
    
            // DETECT LEVER STATE
            lever_state = digitalRead(lever_press);
            
            // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
            if ((lever_state == HIGH) && (press_lapse == 0)) {
              press_lapse = 1;
              Serial.println("LEVER -> ON");
              LP_MIN ++;
              delay(20);
          
            } else if ((lever_state == HIGH) && (press_lapse == 1)){
              // DO NOTHING
            } else if ((lever_state == LOW) && (press_lapse == 1)) {
              press_lapse = 0;
            } else {}
    
  }

}
