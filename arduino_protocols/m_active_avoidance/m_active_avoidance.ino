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
// CHECK SENSORS VARIABLES
// Reading arrays
const unsigned int numReadings = 400;                        // How many readings from each sensor
bool test_pass = true;

// Timing variables for yellow light blink
unsigned long yellow_LED_end_time = 0;
const long blink_interval = 1000;
int yellow_state = LOW;
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

// IR LED Lights
const int speaker_led_r = 9;
const int speaker_led_l = 10;

// LED Check Lights (for UX design)
const int check_red_LED = 47;
const int check_yellow_LED = 49;
const int check_green_LED = 51;
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
SharpIR IR_SENSOR_R1 = SharpIR(ir_right, model);
SharpIR IR_SENSOR_R2 = SharpIR(ir_right2, model);
SharpIR IR_SENSOR_R3 = SharpIR(ir_right3, model);
SharpIR IR_SENSOR_R4 = SharpIR(ir_right4, model);

// Left Sensors
SharpIR IR_SENSOR_L1 = SharpIR(ir_left, model);
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

unsigned int IR_THRESHOLDS[] = {IR_THRESHOLD_L1, IR_THRESHOLD_L2, IR_THRESHOLD_L3, IR_THRESHOLD_L4,
                                IR_THRESHOLD_R1, IR_THRESHOLD_R2, IR_THRESHOLD_R3, IR_THRESHOLD_R4};
//##################################################################################################################



void setup() {

  // INITIATE SERIAL
  Serial.begin(9600);

  SPEAKER_RIGHT.begin(buzzer_pin_r);
  SPEAKER_LEFT.begin(buzzer_pin_l);

  pinMode(shocker_r_pin, OUTPUT);
  pinMode(shocker_l_pin, OUTPUT);

  pinMode(speaker_led_r, OUTPUT);
  pinMode(speaker_led_l, OUTPUT);

  pinMode(check_red_LED, OUTPUT);
  pinMode(check_yellow_LED, OUTPUT);
  pinMode(check_green_LED, OUTPUT);

  // PRINT ENTRY MESSAGE
  delay(5000);
  Serial.println("CHECK SENSOR READINGS...");
  Serial.println("IF THE LIGHT IS: ");
  Serial.print("\t"); Serial.println("SOLID RED, PLEASE RESET THE ARDUINO BOARD");
  Serial.print("\t"); Serial.println("BLINKING YELLOW, THE BOARD IS CHECKING THE SENSORS");
  Serial.print("\t"); Serial.println("GREEN, CONTINUE WITH THE EXPERIMENT :)");
  Serial.print("\t"); Serial.println("BLINKING RED, SENSOR CHECK HAS FAILED :(");

  // CHECK Sensors
  // Turn off LED lights except for reed
  digitalWrite(check_red_LED, HIGH);
  digitalWrite(check_yellow_LED, LOW);
  digitalWrite(check_green_LED, LOW);

  // PRINT MESSAGE TO USER
  Serial.println();
  Serial.println("COLLECTING AND EVALUATING SENSOR READINGS...");
  Serial.println();

  // COLLECT 400 SENSOR READINGS
  // Create empty arrays
  unsigned int checkR1[numReadings];
  unsigned int checkR2[numReadings];
  unsigned int checkR3[numReadings];
  unsigned int checkR4[numReadings];

  unsigned int checkL1[numReadings];
  unsigned int checkL2[numReadings];
  unsigned int checkL3[numReadings];
  unsigned int checkL4[numReadings];

  // Fill arrays
  for (int i = 0; i < numReadings; i++){
    checkR1[i] = IR_SENSOR_R1.distance();
    checkR2[i] = IR_SENSOR_R2.distance();
    checkR3[i] = IR_SENSOR_R3.distance();
    checkR4[i] = IR_SENSOR_R4.distance();

    checkL1[i] = IR_SENSOR_L1.distance();
    checkL2[i] = IR_SENSOR_L2.distance();
    checkL3[i] = IR_SENSOR_L3.distance();
    checkL4[i] = IR_SENSOR_L4.distance();

    // Blink yellow LED
    unsigned long yellow_LED_start_time = millis();
    if (yellow_LED_start_time - yellow_LED_end_time >= blink_interval) {
      yellow_LED_end_time = yellow_LED_start_time;
      if (yellow_state == LOW){
        yellow_state = HIGH;
      } else{
        yellow_state = LOW;
      }
      digitalWrite(check_yellow_LED, yellow_state);
    }
  }

  // FIND MIN VALUES
  // Create min variables
  unsigned int minR1 = checkR1[0];
  unsigned int minR2 = checkR2[0];
  unsigned int minR3 = checkR3[0];
  unsigned int minR4 = checkR4[0];

  unsigned int minL1 = checkL1[0];
  unsigned int minL2 = checkL2[0];
  unsigned int minL3 = checkL3[0];
  unsigned int minL4 = checkL4[0];

  for (int i = 0; i < numReadings; i++){
    // Documentation: https://www.arduino.cc/reference/en/language/functions/math/min/
    minR1 = min(checkR1[i], minR1);
    minR2 = min(checkR2[i], minR2);
    minR3 = min(checkR3[i], minR3);
    minR4 = min(checkR4[i], minR4);

    minL1 = min(checkL1[i], minL1);
    minL2 = min(checkL2[i], minL2);
    minL3 = min(checkL3[i], minL3);
    minL4 = min(checkL4[i], minL4);
  }

  // COMPARE MIN VALUES WITH IR THRESHOLDS SET
  unsigned int arrayMin[8] = {minL1, minL2, minL3, minL4,
                              minR1, minR2, minR3, minR4};

  for (int i = 0; i < (sizeof(arrayMin) / sizeof(arrayMin[0])); i++){
    // The minimum found using 400 values is always greater than the true minimum.
    // We subtract 2 from the minimum found using 400 values to approximate the true minimum.
    arrayMin[i] -= 2;
    if (arrayMin[i] < IR_THRESHOLDS[i]){
      test_pass = false;
    }
  }

  // TURN ON LED LIGHT BASED ON CHECK SENSOR OUTCOME
  if (test_pass){
    // Message to user
    Serial.println("Sensor check complete! Continue with the experiment.");

    // Turn on LEDs
    digitalWrite(check_green_LED, HIGH);
    digitalWrite(check_yellow_LED, LOW);
    digitalWrite(check_red_LED, LOW);

  } else if (!test_pass){
    bool TEST_START = false;

    // Message to user
    Serial.println("Sensor check has failed. Please contact either Rodrigo or Audrey.");

    // Turn on LEDs
    digitalWrite(check_red_LED, LOW);
    digitalWrite(check_yellow_LED, LOW);
    int ledState = LOW;
    while(true){
      if (ledState == LOW){
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(check_red_LED, ledState);
      delay(500);

      // Recover minimum values from readings
      int x = Serial.parseInt();
      if (x==2){
        TEST_START = true;
      }

      if (TEST_START){
        // Reset serial input from Bonsai
        x = 0;
        TEST_START = false;

        Serial.println("Minimum sensor value readings: ");
        Serial.println("L1 L2 L3 L4 R1 R2 R3 R4");
        for (int i = 0; i < (sizeof(arrayMin) / sizeof(arrayMin[0])); i++){
          Serial.print(arrayMin[i]); Serial.print(" ");
        }
        Serial.println();
        Serial.println("Sensor thresholds set: ");
        Serial.println("L1 L2 L3 L4 R1 R2 R3 R4");
        for (int i = 0; i < (sizeof(IR_THRESHOLDS) / sizeof(IR_THRESHOLDS[0])); i++){
          Serial.print(IR_THRESHOLDS[i]); Serial.print(" ");
        }
        Serial.println();
      }
    }

  } else {
    digitalWrite(check_yellow_LED, HIGH);
  }

  /*
  // TEST SENSORS
  // uncomment when you want to test
  while (true){
    Serial.print("Left 1: ");
    Serial.println(IR_SENSOR_L1.distance());
    Serial.print("Left 2: ");
    Serial.println(IR_SENSOR_L2.distance());
    Serial.print("Left 3: ");
    Serial.println(IR_SENSOR_L3.distance());
    Serial.print("Left 4: ");
    Serial.println(IR_SENSOR_L4.distance());

    Serial.print("Right 1: ");
    Serial.println(IR_SENSOR_R1.distance());
    Serial.print("Right 2: ");
    Serial.println(IR_SENSOR_R2.distance());
    Serial.print("Right 3: ");
    Serial.println(IR_SENSOR_R3.distance());
    Serial.print("Right 4: ");
    Serial.println(IR_SENSOR_R4.distance());

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

        if (IR_SENSOR_R1.distance() < IR_THRESHOLD_R1 ||
        IR_SENSOR_R2.distance() < IR_THRESHOLD_R2 ||
        IR_SENSOR_R3.distance() < IR_THRESHOLD_R3 ||
        IR_SENSOR_R4.distance() < IR_THRESHOLD_R4) {
          RIGHT_ACTIVE = HIGH;
          LEFT_ACTIVE = LOW;
        } else if (IR_SENSOR_L1.distance() < IR_THRESHOLD_L1 ||
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
            if (IR_SENSOR_L1.distance() < IR_THRESHOLD_L1 ||
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

  // CHANGE LED TO INDICATE RESET STATUS
  digitalWrite(check_green_LED, LOW);
  digitalWrite(check_red_LED, HIGH);


  }

}
