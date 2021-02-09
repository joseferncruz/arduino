/*
 * LeDoux Lab - DEC 2020
 * jose [dot] cruz [at] nyu [dot] edu
 * 
 */

// VARIABLES
//#########################################################
const int N_TRIALS = 20;
unsigned long ACCLIMATION_DURATION = 20;                       // SECONDS
unsigned long TONE_DURATION = 15;                              // SECONDS 
unsigned long SHOCK_DURATION = 1;                              // SECONDS
int ITI_INTERVALS[] = {40, 60, 80, 100, 120};                  // list of the inter-trial-intervals: ITI

// LOCATION VARIABLES.
// ########################################################
int LEFT_ACTIVE;                                        // TRUE IF A COMPARTMENT IS ACTIVE, ELSE FALSE
int RIGHT_ACTIVE;

// TIME VARIABLES
// ######################################
unsigned long CURRENT_TONE_DELAY;
unsigned long START_TONE;
unsigned long DELTA_TONE_SHOCK = TONE_DURATION - SHOCK_DURATION;
unsigned long ITI_DURATION;

// DIGITAL PINS
// ########################################################
const int speaker_pin = 3;

const int shocker_r_pin = 4;
const int shocker_l_pin = 5;

#include <SharpIR.h>
#define ir_right A0
#define ir_left A1
#define model 1080
SharpIR IR_SENSOR_R = SharpIR(ir_right, model);
SharpIR IR_SENSOR_L = SharpIR(ir_left, model);
int IF_THRESHOLD = 22;                                   // CM > DISTANCE FROM SENSOR TO OPPOSITE WALL.

const int speaker_led_r = 9;
const int speaker_led_l = 10;

const int start_switch_pin = 22;
const int test_switch_pin = 23;


// VARIABLES FOR STATISTICS
// ##########################################################
unsigned long ESCAPE_LATENCY_START;
unsigned long ESCAPE_LATENCY_END;
unsigned long ESCAPE_LATENCY_DELTA;
float ESCAPE_LATENCY_CUMULATIVE;

// SESSION
int TOTAL_AVOIDANCE_SUCCESS = 0;                           // CUMULATIVE COUNT OF SUCCESSFUL AVOIDANCE RESPONSES
int TOTAL_AVOIDANCE_FAILURE = 0;                           // CUMULATIVE COUNT OF FAILED AVOIDANCE RESPONSES
//int ESCAPE_LATENCY_INDIVIDUAL[N_TRIALS];                   // LIST WITH THE LATENCY. 0 == NO SHUTTLE (== FAILURE)


void setup() {

  // INITIATE SERIAL
  Serial.begin(9600);
  
  // ASSIGN PINS 
  pinMode(speaker_pin, OUTPUT);
  pinMode(shocker_r_pin, OUTPUT);
  pinMode(shocker_l_pin, OUTPUT);
  
  pinMode(speaker_led_r, OUTPUT);
  pinMode(speaker_led_l, OUTPUT);
  
  pinMode(start_switch_pin, INPUT);
  pinMode(test_switch_pin, INPUT);

  // PRINT ENTRY MESSAGE 
  Serial.println("PRESS GREEN SWITCH TO START...");


  // UNCOMMENT TO TEST SENSORS
//  while (true) {
//    Serial.print("Left Sensor: ");
//    Serial.println(IR_SENSOR_L.distance());
//    Serial.print("Right Sensor: ");
//    Serial.println(IR_SENSOR_R.distance());
//    delay(1000);
//  }

}

void loop() {

  bool SESSION_START = false;
  bool TEST_START = false;

  // TRIGGER START OF TRIAL VIA BONSAI-RX
  int x = Serial.parseInt();
  if (x == 1) {
    SESSION_START = true;
  } else if (x == 2) {
    TEST_START = true;
  } else {
    // NOTHING
  }
  

  // TEST CHAMBER
  if (TEST_START) {
    
    // RESET SERIAL INPUT FROM BONSAI-RX
    x = 0;
    TEST_START = false;
    
    // TEST LEDs
    Serial.println("TEST CHAMBER LEDs");
    digitalWrite(speaker_led_r, HIGH);
    digitalWrite(speaker_led_l, HIGH);
    delay(3000);
    digitalWrite(speaker_led_r, LOW);
    digitalWrite(speaker_led_l, LOW);

    // TEST TONE GENERATION
    Serial.println("TEST TONE GENERATION");
    digitalWrite(speaker_pin, HIGH);
    delay(3000);
    digitalWrite(speaker_pin, LOW);  
    
    // TEST SHOCK GENERATION LEFT, THEN RIGHT
    Serial.println("TEST SHOCKER");
    Serial.println("RIGHT SIDE");
    digitalWrite(shocker_r_pin, HIGH);
    delay(3000);
    digitalWrite(shocker_r_pin, LOW);    
    Serial.println("LEFT SIDE");
    digitalWrite(shocker_l_pin, HIGH);
    delay(3000);
    digitalWrite(shocker_l_pin, LOW);
    
    // TEST PIR SENSOR LEFT, THEN RIGHT
    
  }

  // RESET CUMMULATIVE VARIABLE VALUES
  TOTAL_AVOIDANCE_FAILURE = 0;
  TOTAL_AVOIDANCE_SUCCESS = 0;
  ESCAPE_LATENCY_CUMULATIVE = 0;

  // START SESSION
  if (SESSION_START) {

    // RESET SERIAL INPUT FROM BONSAI-RX
    x = 0;
    TEST_START = false;
    

    // PRINT BASIC SESSION INFORMATION
    // ###############################
    Serial.println("ACTIVE AVOIDANCE");
    Serial.print("NUMBER OF TRIALS: "); Serial.println(N_TRIALS); 
    Serial.print("TONE DURATION (SEC): "); Serial.println(TONE_DURATION);
    Serial.print("SHOCK DURATION (SEC): "); Serial.println(SHOCK_DURATION);
    Serial.print("TONE PAIRED WITH SHOCK AT (SEC): "); Serial.println(TONE_DURATION - SHOCK_DURATION);

    // SIGNAL START OF THE SESSION
    for (int x = 0; x < 5; x ++) {
      
      // TURN LED ON IN BOTH SIDES
      digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);
      delay(500);
      
      // TURN LED OFF IN BOTH SIDES
      digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
      delay(500);
    }
    Serial.println("SESSION > START");

    // ACCLIMATION PERIOD
    // ##################
    Serial.print("ACCLIMATION (SEC): "); Serial.println(ACCLIMATION_DURATION);
    delay(ACCLIMATION_DURATION*1000);

    // START TRIALS
    for (int x = 0; x < N_TRIALS; x++) {

      // PRINT INFORMATION ABOUT TRIAL
      Serial.print("TRIAL NUMBER "); Serial.print(x+1); Serial.println(" > START");


      // TRIAL ONE UNAVOID
      if (x == 0) {

          // TURN THE SPEAKER ON
          digitalWrite(speaker_pin, HIGH);
          Serial.println("CS > ON");

          // TURN LED ON IN BOTH SIDES
          digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);

          // RECORD START OF THE TONE
          START_TONE = millis();
        
        while (true) {
          
          CURRENT_TONE_DELAY = millis();

          if ((CURRENT_TONE_DELAY - START_TONE) > (DELTA_TONE_SHOCK * 1000)) {

            // TRIGGER US
            digitalWrite(shocker_l_pin, HIGH);
            Serial.println("US_L > ON");
            digitalWrite(shocker_r_pin, HIGH);
            Serial.println("US_R > ON");

            // KEEP US FOR SPECIFIC TIME DELAY
            for (int i = 0; i < SHOCK_DURATION; i++) {
              delay(1000);
            }

            // TERMINATE SHOCKER
            digitalWrite(shocker_l_pin, LOW);
            Serial.println("US_L > OFF");
            digitalWrite(shocker_r_pin, LOW);
            Serial.println("US_R > OFF");
            
            // TERMINATE TONE IN THE COMPARTMENT IF AFTER SHOCK
            digitalWrite(speaker_pin, LOW);
            Serial.println("CS > OFF");

            // RECORD LATENCY_END WHEN NO SHUTTLING
            ESCAPE_LATENCY_END = 0;

            // COUNT ONE TOWARDS AVOIDANCE FAILURE
            TOTAL_AVOIDANCE_FAILURE ++;

            // TURN LED OFF IN BOTH SIDES
            digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
            
            break;
          }

        }

        // SELECT RANDOM ITI FROM LIST
        ITI_DURATION = ITI_INTERVALS[random(0, 5)];
        Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(ITI_DURATION);

        // INITIATE INTER-TRIAL-INTERVAL 
        delay(ITI_DURATION*1000);
  
        // PRINT EXIT INFORMATION ABOUT TRIAL
        // ###########################################################################
        Serial.print("TRIAL "); Serial.print(x+1); Serial.println(" > END");
        Serial.print("TRIAL "); Serial.print(x+1); Serial.println(" ESCAPE LATENCY (SEC): 0");  // FIRST IS UNAVOID
        Serial.print("CUMULATIVE TOTAL SHUTTLINGS: "); Serial.println(TOTAL_AVOIDANCE_SUCCESS); 
        Serial.print("CUMULATIVE TOTAL FAILURES: "); Serial.println(TOTAL_AVOIDANCE_FAILURE);

        // CONTINUE TO THE NEXT TRIAL
        continue;
        
      }
      
      // DETECT POSITION, DELIVER CS AND US        
      while (true) {

        if (IR_SENSOR_R.distance() < IF_THRESHOLD) {
          RIGHT_ACTIVE = HIGH;
          LEFT_ACTIVE = LOW;
        } else if (IR_SENSOR_L.distance() < IF_THRESHOLD) {
          LEFT_ACTIVE = HIGH;
          RIGHT_ACTIVE = LOW;
        } else {
          RIGHT_ACTIVE = LOW;
          LEFT_ACTIVE = LOW;
        }

        // RESET VARIABLES FOR SHUTTLING
        ESCAPE_LATENCY_START = 0;
        ESCAPE_LATENCY_END = 0;
        
        if (RIGHT_ACTIVE) {

          // DELIVER TONE IN THE RIGHT COMPARTMENT
          Serial.println("RIGHT COMPARTMENT > ACTIVE");
          digitalWrite(speaker_pin, HIGH);
          Serial.println("CS > ON");

          // RECORD LATENCY_START
          ESCAPE_LATENCY_START = millis();

          // TURN LED ON IN BOTH SIDES
          digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);
          
          START_TONE = millis();
          CURRENT_TONE_DELAY = millis();

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (true) {

            // CHECK IF LEFT IS ACTIVE
            if (IR_SENSOR_L.distance() < IF_THRESHOLD) {
              LEFT_ACTIVE = HIGH;
            } else {
              LEFT_ACTIVE = LOW;
            }
            
            CURRENT_TONE_DELAY = millis();
            
            if (LEFT_ACTIVE == HIGH) {

              // IF THE LEFT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE IN THE RIGHT COMPARTMENT
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
              break;
              
            }

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((CURRENT_TONE_DELAY - START_TONE) > (DELTA_TONE_SHOCK * 1000)) {

              // TRIGGER US
              digitalWrite(shocker_r_pin, HIGH);
              Serial.println("US_R > ON");

              // KEEP US FOR SPECIFIC TIME DELAY
              for (int i = 0; i < SHOCK_DURATION; i++) {
                delay(1000);
              }

              // TERMINATE SHOCK IN THE RIGHT COMPARTMENT
              digitalWrite(shocker_r_pin, LOW);
              Serial.println("US_R > OFF");
              
              // TERMINATE TONE IN THE RIGHT COMPARTMENT IF AFTER SHOCK
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN NO SHUTTLING
              ESCAPE_LATENCY_END = ESCAPE_LATENCY_START;

              // COUNT ONE TOWARDS AVOIDANCE FAILURE
              TOTAL_AVOIDANCE_FAILURE ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
              break;
            }
          }

          break;     
         
        } else if (LEFT_ACTIVE) {
          
          // DELIVER TONE IN THE LEFT COMPARTMENT
          Serial.println("LEFT COMPARTMENT > ACTIVE");
          digitalWrite(speaker_pin, HIGH);
          START_TONE = millis();
          CURRENT_TONE_DELAY = millis();
          Serial.println("CS > ON");

          // RECORD LATENCY_START
          ESCAPE_LATENCY_START = millis();

          // TURN LED ON IN BOTH SIDES
          digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (true) {

            // CHECK IF RIGHT IS ACTIVE
            if (IR_SENSOR_R.distance() < IF_THRESHOLD) {
              RIGHT_ACTIVE = HIGH;
            } else {
              RIGHT_ACTIVE = LOW;
            }
            
            if (RIGHT_ACTIVE == HIGH) {

              // IF THE RIGHT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE 
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
      
              break;
            }

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((CURRENT_TONE_DELAY - START_TONE) > (DELTA_TONE_SHOCK * 1000)) {

              // TRIGGER US
              digitalWrite(shocker_l_pin, HIGH);
              Serial.println("US_L > ON");

              // KEEP US FOR SPECIFIC TIME DELAY
              for (int i = 0; i < SHOCK_DURATION; i++) {
                delay(1000);
              }

              // TERMINATE SHOCKER
              digitalWrite(shocker_l_pin, LOW);
              Serial.println("US_L > OFF");
              
              // TERMINATE TONE IN THE COMPARTMENT IF AFTER SHOCK
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN NO SHUTTLING
              ESCAPE_LATENCY_END = ESCAPE_LATENCY_START;

              // COUNT ONE TOWARDS AVOIDANCE FAILURE
              TOTAL_AVOIDANCE_FAILURE ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
              
              break;
            }
            CURRENT_TONE_DELAY = millis();
          }
          break;
          

        } else {
          // IN THE ABSSENCE OF MOVEMENT IN EITHER COMPARTMENT, DO NOTHING
        }

      }

      // SELECT RANDOM ITI FROM LIST
      ITI_DURATION = ITI_INTERVALS[random(0, 5)];
      Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(ITI_DURATION);
      
      // INITIATE INTER-TRIAL-INTERVAL 
      delay(ITI_DURATION*1000);


      // PRINT EXIT INFORMATION ABOUT TRIAL
      // ###########################################################################
      Serial.print("TRIAL "); Serial.print(x+1); Serial.println(" > END");
      Serial.print("TRIAL "); Serial.print(x+1); Serial.print(" ESCAPE LATENCY (SEC): "); Serial.println((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000.0);
      Serial.print("CUMULATIVE TOTAL SHUTTLINGS: "); Serial.println(TOTAL_AVOIDANCE_SUCCESS); 
      Serial.print("CUMULATIVE TOTAL FAILURES: "); Serial.println(TOTAL_AVOIDANCE_FAILURE);
      
    }

    // SESSION FINAL INFO AND STATISTICS
    // ###########################################################################
    Serial.println("SESSION > END");
    Serial.println("SESSION STATISTICS");
    Serial.print("SESSION TOTAL SHUTTLINGS: "); Serial.println(TOTAL_AVOIDANCE_SUCCESS);
    Serial.print("SESSION TOTAL FAILURES: "); Serial.println(TOTAL_AVOIDANCE_FAILURE);
    if (TOTAL_AVOIDANCE_SUCCESS == 0) {
      Serial.print("MEAN SHUTTLING LATENCY (SEC): 0");
    } else {
      Serial.print("SESSION MEAN SHUTTLING LATENCY (SEC): "); Serial.println((float)ESCAPE_LATENCY_CUMULATIVE / (float)TOTAL_AVOIDANCE_SUCCESS);
    }
    

   
  }

}
