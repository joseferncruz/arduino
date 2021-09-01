/*
 * LeDoux Lab - DEC 2021
 * jose [dot] cruz [at] nyu [dot] edu
 * ay2376 [at] nyu [dot] edu
 * snm427 [at] nyu [dot] edu
 */


 /*
    LIBRARIES
 */
 #include <SharpIR.h>
 #include <Tone.h>


/*
    VARIABLES
*/
//##################################################################################################################
// EXPERIMENTAL VARIABLES
const int N_TRIALS = 20;
unsigned long ACCLIMATION_DURATION = 20;                       // SECONDS
unsigned long TONE_DURATION = 15;                              // SECONDS
unsigned long SHOCK_DURATION = 1;                              // SECONDS
int CS_FREQUENCY = 5000;                                       // IN HERTZ
int ITI_INTERVALS[] = {40, 60, 80, 100, 120};                  // list of the inter-trial-intervals: ITI
//##################################################################################################################
// LOCATION VARIABLES
int LEFT_ACTIVE;                                               // HIGH IF A COMPARTMENT IS ACTIVE, ELSE LOW
int RIGHT_ACTIVE;
//##################################################################################################################
// TIMING VARIABLES
unsigned long CURRENT_TONE_DELAY;
unsigned long START_TONE;
unsigned long DELTA_TONE_SHOCK = TONE_DURATION - SHOCK_DURATION;
unsigned long ITI_DURATION;
//##################################################################################################################
// VARIABLES FOR STATISTICS
unsigned long ESCAPE_LATENCY_START;
unsigned long ESCAPE_LATENCY_END;
unsigned long ESCAPE_LATENCY_DELTA;
float ESCAPE_LATENCY_CUMULATIVE;

// SESSION
int TOTAL_AVOIDANCE_SUCCESS = 0;                           // CUMULATIVE COUNT OF SUCCESSFUL AVOIDANCE RESPONSES
int TOTAL_AVOIDANCE_FAILURE = 0;                           // CUMULATIVE COUNT OF FAILED AVOIDANCE RESPONSES
//int ESCAPE_LATENCY_INDIVIDUAL[N_TRIALS];                   // LIST WITH THE LATENCY. 0 == NO SHUTTLE (== FAILURE)
//##################################################################################################################


/*
    PIN ASSIGNMENTS
*/
//##################################################################################################################
// DIGITAL PINS
// Speakers
const int speaker_pin = 3;

// Shockers
const int shocker_r_pin = 4;
const int shocker_l_pin = 5;

// Buzzers
const int buzzer_pin_r = 6;
const int buzzer_pin_l = 7;

// LED Lights
const int speaker_led_r = 9;
const int speaker_led_l = 10;
//##################################################################################################################
// ANALOG PINS
// Right Sensors
#define ir_right A0
#define ir_right2 A2
#define ir_right3 A4
#define ir_right4 A6

// Left Sensors
#define ir_left A1
#define ir_left2 A3
#define ir_left3 A5
#define ir_left4 A7
//##################################################################################################################


/*
    INITIALIZING LIBRARIES
*/
//##################################################################################################################
// TONE LIBRARY
Tone SPEAKER_RIGHT;
Tone SPEAKER_LEFT;
//##################################################################################################################
// IR SENSOR LIBRARY
#define model 1080

// Right Sensors
SharpIR IR_SENSOR_R = SharpIR(ir_right, model);
SharpIR IR_SENSOR_R2 = SharpIR(ir_right2, model);
SharpIR IR_SENSOR_R3 = SharpIR(ir_right3, model);
SharpIR IR_SENSOR_R4 = SharpIR(ir_right4, model);

// Left Sensors
SharpIR IR_SENSOR_L = SharpIR(ir_left, model);
SharpIR IR_SENSOR_L2 = SharpIR(ir_left2, model);
SharpIR IR_SENSOR_L3 = SharpIR(ir_left3, model);
SharpIR IR_SENSOR_L4 = SharpIR(ir_left4, model);

// SENSOR THRESHOLDS
const int IR_THRESHOLD_L1 = 19;
const int IR_THRESHOLD_L2 = 18;
const int IR_THRESHOLD_L3 = 18;
const int IR_THRESHOLD_L4 = 20;

const int IR_THRESHOLD_R1 = 22;
const int IR_THRESHOLD_R2 = 21;
const int IR_THRESHOLD_R3 = 22;
const int IR_THRESHOLD_R4 = 19;
//##################################################################################################################



void setup() {

  // INITIATE SERIAL
  Serial.begin(9600);

  // ASSIGN PINS
//  pinMode(speaker_pin, OUTPUT);

  SPEAKER_RIGHT.begin(buzzer_pin_r);
  SPEAKER_LEFT.begin(buzzer_pin_l);

  pinMode(shocker_r_pin, OUTPUT);
  pinMode(shocker_l_pin, OUTPUT);

  pinMode(speaker_led_r, OUTPUT);
  pinMode(speaker_led_l, OUTPUT);

  pinMode(start_switch_pin, INPUT);
  pinMode(test_switch_pin, INPUT);

  // PRINT ENTRY MESSAGE
  Serial.println("PRESS GREEN SWITCH TO START...");


  /*
  // TEST SENSORS
  // uncomment when you want to test
  while (true){
    Serial.print("Left 1: ");
    Serial.println(IR_SENSOR_L.distance());
    Serial.print("Left 2: ");
    Serial.println(IR_SENSOR_L2.distance());
    Serial.print("Left 3: ");
    Serial.println(IR_SENSOR_L3.distance());
    Serial.print("Left 4: ");
    Serial.println(IR_SENSOR_L4.distance());
   // Serial.print("Left 5: ");
   // Serial.println(IR_SENSOR_L5.distance());

    Serial.print("Right 1: ");
    Serial.println(IR_SENSOR_R.distance());
    Serial.print("Right 2: ");
    Serial.println(IR_SENSOR_R2.distance());
    Serial.print("Right 3: ");
    Serial.println(IR_SENSOR_R3.distance());
    Serial.print("Right 4: ");
    Serial.println(IR_SENSOR_R4.distance());
    // Serial.print("Right 5: ");
    // Serial.println(IR_SENSOR_R5.distance());

    delay(2000);
    }
    /**/


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
    SPEAKER_RIGHT.play(CS_FREQUENCY);
    SPEAKER_LEFT.play(CS_FREQUENCY);
    delay(3000);
    SPEAKER_RIGHT.stop();
    SPEAKER_LEFT.stop();

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
//          digitalWrite(speaker_pin, HIGH);
          SPEAKER_RIGHT.play(CS_FREQUENCY);                        // FREQUENCY
          SPEAKER_LEFT.play(CS_FREQUENCY);                         // FREQUENCY
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
//            digitalWrite(speaker_pin, LOW);
            SPEAKER_RIGHT.stop();
            SPEAKER_LEFT.stop();
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

        // RESET ACTIVE CHAMBER SIDE SENSOR VARIABLES
        RIGHT_ACTIVE = LOW;
        LEFT_ACTIVE = LOW;

        // CONTINUE TO THE NEXT TRIAL
        continue;

      }

      // DETECT POSITION, DELIVER CS AND US
      while (true) {

        if (IR_SENSOR_R.distance() < IR_THRESHOLD_R1 ||
        IR_SENSOR_R2.distance() < IR_THRESHOLD_R2 ||
        IR_SENSOR_R3.distance() < IR_THRESHOLD_R3 ||
        IR_SENSOR_R4.distance() < IR_THRESHOLD_R4) {
          RIGHT_ACTIVE = HIGH;
          LEFT_ACTIVE = LOW;
        } else if (IR_SENSOR_L.distance() < IR_THRESHOLD_L1 ||
        IR_SENSOR_L2.distance() < IR_THRESHOLD_L2 ||
        IR_SENSOR_L3.distance() < IR_THRESHOLD_L3 ||
        IR_SENSOR_L4.distance() < IR_THRESHOLD_L4) {
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
          SPEAKER_RIGHT.play(CS_FREQUENCY);
          SPEAKER_LEFT.play(CS_FREQUENCY);
          Serial.println("CS > ON");

          // RECORD LATENCY_START
          ESCAPE_LATENCY_START = millis();

          // TURN LED ON IN BOTH SIDES
          digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);

          START_TONE = millis();
          CURRENT_TONE_DELAY = millis();

          // ADD 0.5 SEC DELAY TO AVOID SENSOR DETECTION ARTIFACTS
          unsigned long S_DELAY_START = millis();
          unsigned long S_DELAY_CURRENT = millis();
          while (S_DELAY_START - S_DELAY_CURRENT < 500) {
            S_DELAY_CURRENT = millis();
          }

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (true) {

            // CHECK IF LEFT IS ACTIVE
            if (IR_SENSOR_L.distance() < IR_THRESHOLD_L1 ||
            IR_SENSOR_L2.distance() < IR_THRESHOLD_L2 ||
            IR_SENSOR_L3.distance() < IR_THRESHOLD_L3) {
              LEFT_ACTIVE = HIGH;
              RIGHT_ACTIVE = LOW;
            } else {
              LEFT_ACTIVE = LOW;
              RIGHT_ACTIVE = LOW;
            }

            CURRENT_TONE_DELAY = millis();

            if (LEFT_ACTIVE == HIGH) {

              // IF THE LEFT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE IN THE RIGHT COMPARTMENT
              SPEAKER_RIGHT.stop();
              SPEAKER_LEFT.stop();
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);

              // RESET LEFT SENSOR VALUE
              LEFT_ACTIVE = LOW;

              // CONTINUE TO THE NEXT TRIAL
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
              SPEAKER_RIGHT.stop();
              SPEAKER_LEFT.stop();
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
          SPEAKER_RIGHT.play(CS_FREQUENCY);
          SPEAKER_LEFT.play(CS_FREQUENCY);
          START_TONE = millis();
          CURRENT_TONE_DELAY = millis();
          Serial.println("CS > ON");

          // RECORD LATENCY_START
          ESCAPE_LATENCY_START = millis();

          // TURN LED ON IN BOTH SIDES
          digitalWrite(speaker_led_r, HIGH); digitalWrite(speaker_led_l, HIGH);

          // ADD 0.5 SEC DELAY TO AVOID SENSOR DETECTION ARTIFACTS
          unsigned long S_DELAY_START = millis();
          unsigned long S_DELAY_CURRENT = millis();
          while (S_DELAY_START - S_DELAY_CURRENT < 500) {
            S_DELAY_CURRENT = millis();
          }

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (true) {

            // CHECK IF RIGHT IS ACTIVE
            if (IR_SENSOR_R2.distance() < IR_THRESHOLD_R2 ||
            IR_SENSOR_R3.distance() < IR_THRESHOLD_R3 ||
            IR_SENSOR_R4.distance() < IR_THRESHOLD_R4) {
              RIGHT_ACTIVE = HIGH;
              LEFT_ACTIVE = LOW;
            } else {
              RIGHT_ACTIVE = LOW;
              LEFT_ACTIVE = LOW;
            }

            if (RIGHT_ACTIVE == HIGH) {

              // IF THE RIGHT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE
              SPEAKER_RIGHT.stop();
              SPEAKER_LEFT.stop();
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);

              // RESET RIGHT SENSOR VALUE
              RIGHT_ACTIVE == LOW;

              // CONTINUE TO THE NEXT TRIAL
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
              SPEAKER_RIGHT.stop();
              SPEAKER_LEFT.stop();
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
      Serial.print("SESSION MEAN SHUTTLING LATENCY (SEC): "); Serial.println(TOTAL_AVOIDANCE_SUCCESS);
    } else {
      Serial.print("SESSION MEAN SHUTTLING LATENCY (SEC): "); Serial.println((float)ESCAPE_LATENCY_CUMULATIVE / (float)TOTAL_AVOIDANCE_SUCCESS);
    }



  }

}
