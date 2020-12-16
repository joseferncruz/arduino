


// VARIABLES
//#########################################################

int N_TRIALS = 30;
unsigned long ACCLIMATION_DURATION = 5;                 // SECONDS
unsigned long TONE_DURATION = 15;                        // SECONDS 
unsigned long SHOCK_DURATION = 2;                        // SECONDS


// OTHER
// ########################################################
int ITI_INTERVALS[] = {5, 5, 5, 5, 5};           // list of the inter-trial-intervals: ITI

int LEFT_ACTIVE;                                        // TRUE if occupied, else FALSE
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

const int speaker_led_r = 8;
const int speaker_led_l = 9;


const int start_switch_pin = 22;
const int test_switch_pin = 23;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // STABILIZE PIR SENSOR
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



    // ACCLIMATION PERIOD
    // ####################################
    Serial.print("ACCLIMATION(SEC):"); Serial.println(ACCLIMATION_DURATION);
    delay(ACCLIMATION_DURATION*1000);

    // START TRIALS
    for (int x = 0; x < N_TRIALS; x++) {

      // PRINT INFORMATION ABOUT TRIAL
      Serial.print("TRIAL NUMBER: "); Serial.println(x+1);
      


      // DETECT POSITION, DELIVER CS AND US        
      while (true) {


//        // DELAY FOR PIR SENSOR TO STABILIZE
        delay(500);
        
        RIGHT_ACTIVE = digitalRead(pir_r_pin);
        LEFT_ACTIVE = digitalRead(pir_l_pin);

        
        
        
        if (RIGHT_ACTIVE) {

          // DELIVER TONE IN THE RIGHT COMPARTMENT
          Serial.println("RIGHT COMPARTMENT > ACTIVE");
          digitalWrite(speaker_pin, HIGH);
          Serial.println("CS > ON");
          
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
              break;
              
            }

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((CURRENT_TONE_DELAY - START_TONE) > (TONE_DURATION * 1000)) {

              // TRIGGER US
              digitalWrite(shocker_r_pin, HIGH);
              Serial.println("US_L > ON");

              // KEEP US FOR SPECIFIC TIME DELAY
              for (int i = 0; i < SHOCK_DURATION; i++) {
                delay(1000);
              }

              // TERMINATE SHOCK IN THE RIGHT COMPARTMENT
              digitalWrite(shocker_r_pin, LOW);
              Serial.println("US_L > OFF");
              
              // TERMINATE TONE IN THE RIGHT COMPARTMENT IF AFTER SHOCK
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");
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

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (true) {

            // CHECK IF LEFT IS ACTIVE
            RIGHT_ACTIVE = digitalRead(pir_r_pin);
            
            if (RIGHT_ACTIVE == HIGH) {

              // IF THE RIGHT IS HIGH THEN TERMINATE TONE AND MOVE TO ITI
              // TERMINATES TONE 
              digitalWrite(speaker_pin, LOW);
              Serial.println("CS > OFF");
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
              break;
            }
            CURRENT_TONE_DELAY = millis();
          }
          break;
          

        } else {
          // NOTHING
        }

//       // AFTER CS US PROTOCOL, BREAK OUT OF THE LOOP AND INITIATE ITI
//       break;
      }

      // SELECT RANDOM ITI FROM LIST
      int ITI_DURATION = ITI_INTERVALS[random(0, 5)];
      Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(ITI_DURATION);
      
      // INITIATE INTER-TRIAL-INTERVAL 
      delay(ITI_DURATION*1000);


      // PRINT EXIT INFORMATION ABOUT TRIAL
      Serial.print("TRIAL NUMBER: "); Serial.println(x+1);
      Serial.println("TRIAL > END");

      
    }


    
  }
  









 

}
