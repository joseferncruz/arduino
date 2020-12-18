/*
 * LeDoux Lab - DEC 2020
 * jose [dot] cruz [at] nyu [dot] edu
 * 
 */


// VARIABLES
//#########################################################
const int N_TRIALS = 5;
unsigned long ACCLIMATION_DURATION = 5;                  // SECONDS
unsigned long TONE_DURATION = 15;                        // SECONDS 
unsigned long SHOCK_DURATION = 2;                        // SECONDS
int ITI_INTERVALS[] = {5, 5, 5, 5, 5};                  // list of the inter-trial-intervals: ITI

// LOCATION VARIABLES.
// ########################################################
int LEFT_ACTIVE;                                        // TRUE IF A COMPARTMENT IS ACTIVE, ELSE FALSE
int RIGHT_ACTIVE;

// TIME VARIABLES
// ######################################
unsigned long CURRENT_TONE_DELAY;
unsigned long START_TONE;

// DIGITAL PINS
// ########################################################
const int speaker_pin = 3;

const int shocker_r_pin = 4;
const int shocker_l_pin = 5;

const int pir_r_pin = 6;
const int pir_l_pin = 7;

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
int ESCAPE_LATENCY_INDIVIDUAL[N_TRIALS];                   // LIST WITH THE LATENCY. 0 == NO SHUTTLE (== FAILURE)


void setup() {

  // INITIATE SERIAL
  Serial.begin(9600);

  // ALLOW PIR SENSOR TO STABILIZE
  Serial.println("INITIATING PIR SENSOR. PLEASE WAIT 30 SECONDS.");
  for (int i = 0; i < 30; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("PIR SENSOR INITIATED.");
  
  // ASSIGN PINS 
  pinMode(speaker_pin, OUTPUT);
  pinMode(shocker_r_pin, OUTPUT);
  pinMode(shocker_l_pin, OUTPUT);
  
  pinMode(pir_r_pin, INPUT);
  pinMode(pir_l_pin, INPUT);

  pinMode(speaker_led_r, OUTPUT);
  pinMode(speaker_led_l, OUTPUT);
  
  pinMode(start_switch_pin, INPUT);
  pinMode(test_switch_pin, INPUT);

  // PRINT ENTRY MESSAGE 
  Serial.println("PRESS GREEN SWITCH TO START...");

}

void loop() {


  // TEST CHAMBER
  if (digitalRead(test_switch_pin) == HIGH) {
    
    // TEST LEDS LEFT AND THEN RIGHT

    // TEST TONE GENERATION LEFT, THEN RIGHT
    
    // TEST SHOCK GENERATION LEFT, THEN RIGHT

    // TEST PIR SENSOR LEFT, THEN RIGHT
  }


  // START SESSION
  if (digitalRead(start_switch_pin) == HIGH) {

    // PRINT BASIC SESSION INFORMATION
    // ###############################
    Serial.println("SESSION > START");
    Serial.print("NUMBER OF TRIALS: "); Serial.println(N_TRIALS); 
    Serial.print("TONE DURATION: "); Serial.println(TONE_DURATION);
    Serial.print("SHOCK DURATION: "); Serial.println(SHOCK_DURATION);


    // ACCLIMATION PERIOD
    // ##################
    Serial.print("ACCLIMATION(SEC): "); Serial.println(ACCLIMATION_DURATION);
    delay(ACCLIMATION_DURATION*1000);

    // START TRIALS
    for (int x = 0; x < N_TRIALS; x++) {

      // PRINT INFORMATION ABOUT TRIAL
      Serial.print("TRIAL NUMBER "); Serial.print(x+1); Serial.println(" > START");
      
      // DETECT POSITION, DELIVER CS AND US        
      while (true) {

        // DELAY FOR PIR SENSOR TO STABILIZE
        delay(500);
        
        RIGHT_ACTIVE = digitalRead(pir_r_pin);
        LEFT_ACTIVE = digitalRead(pir_l_pin);


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
            LEFT_ACTIVE = digitalRead(pir_l_pin);
            CURRENT_TONE_DELAY = millis();
            
            if (LEFT_ACTIVE == HIGH) {

              // IF THE LEFT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE IN THE RIGHT COMPARTMENT
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();
              ESCAPE_LATENCY_INDIVIDUAL[x] = (((float)ESCAPE_LATENCY_END - (float)ESCAPE_LATENCY_START)/1000.0);

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
              break;
              
            }

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((CURRENT_TONE_DELAY - START_TONE) > (TONE_DURATION * 1000)) {

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
              ESCAPE_LATENCY_INDIVIDUAL[x] = (((float)ESCAPE_LATENCY_END - (float)ESCAPE_LATENCY_START)/1000.0);

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

            // CHECK IF LEFT IS ACTIVE
            RIGHT_ACTIVE = digitalRead(pir_r_pin);
            
            if (RIGHT_ACTIVE == HIGH) {

              // IF THE RIGHT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE 
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");

              // RECORD LATENCY_END WHEN SHUTTLING
              ESCAPE_LATENCY_END = millis();
              ESCAPE_LATENCY_INDIVIDUAL[x] = (((float)ESCAPE_LATENCY_END - (float)ESCAPE_LATENCY_START)/1000.0);

              // CUMULATIVE COUNT OF THE LATENCY TO CALCULATE THE MEAN AT THE END OF THE SESSION
              ESCAPE_LATENCY_CUMULATIVE = ESCAPE_LATENCY_CUMULATIVE + ((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);

              // COUNT ONE TOWARDS AVOIDANCE SUCCESS
              TOTAL_AVOIDANCE_SUCCESS ++;

              // TURN LED OFF IN BOTH SIDES
              digitalWrite(speaker_led_r, LOW); digitalWrite(speaker_led_l, LOW);
      
              break;
            }

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((CURRENT_TONE_DELAY - START_TONE) > (TONE_DURATION * 1000)) {

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
              ESCAPE_LATENCY_INDIVIDUAL[x] = (((float)ESCAPE_LATENCY_END - (float)ESCAPE_LATENCY_START)/1000.0);

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
      int ITI_DURATION = ITI_INTERVALS[random(0, 5)];
      Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(ITI_DURATION);
      
      // INITIATE INTER-TRIAL-INTERVAL 
      delay(ITI_DURATION*1000);


      // PRINT EXIT INFORMATION ABOUT TRIAL
      // ###########################################################################
      Serial.print("TRIAL NUMBER "); Serial.print(x+1); Serial.println(" > END");
      Serial.print("ESCAPE LATENCY: "); Serial.println((ESCAPE_LATENCY_END - ESCAPE_LATENCY_START)/1000);
      Serial.print("TOTAL SHUTTLINGS: "); Serial.println(TOTAL_AVOIDANCE_SUCCESS); 
      Serial.print("TOTAL FAILURES: "); Serial.println(TOTAL_AVOIDANCE_FAILURE);
      
    }

    // SESSION FINAL INFO AND STATISTICS
    // ###########################################################################
    Serial.println("SESSION > END");
    Serial.println("SESSION STATISTICS");
    Serial.print("INDIVIDUAL SHUTTLING LATENCY: "); 
    for (int x = 0; x < N_TRIALS; x++) {
      Serial.print(ESCAPE_LATENCY_INDIVIDUAL[x]);
      Serial.print(", ");
      }; Serial.println("");
    Serial.print("MEAN SHUTTLING LATENCY: "); Serial.println((float)ESCAPE_LATENCY_CUMULATIVE / (float)TOTAL_AVOIDANCE_SUCCESS);

   
  }

}
