


// VARIABLES
//#########################################################

int N_TRIALS = 30;
unsigned long ACCLIMATION_DURATION = 60;                 // SECONDS
unsigned long TONE_DURATION = 30;                        // SECONDS 
unsigned long SHOCK_DURATION = 2;                        // SECONDS


// OTHER
// ########################################################
int ITI_INTERVALS[] = {60, 90, 120, 160, 180};           // list of the inter-trial-intervals: ITI

int LEFT_ACTIVE;                                        // TRUE if occupied, else FALSE
int RIGHT_ACTIVE;

// DIGITAL PINS
// ########################################################

const int speaker_r_pin = 2;
const int speaker_l_pin = 3;
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
  Serial.println("INITIATING PIR SENSOR. PLEASE WAIT 60 SECONDS.");
  for (int i = 0; i < 60; i++) {
    delay(1000);
  }
  
  


  // ASSIGN PINS 
  pinMode(speaker_r_pin, OUTPUT);
  pinMode(speaker_l_pin, OUTPUT);
  pinMode(shocker_r_pin, OUTPUT);
  pinMode(shocker_l_pin, OUTPUT);
  
  pinMode(pir_r_pin, INPUT);
  pinMode(pir_l_pin, INPUT);
  
  pinMode(start_switch_pin, INPUT);
  pinMode(test_switch_pin, INPUT);
  



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
    Serial.print("ACCLIMATION (SEC)"); Serial.println(ACCLIMATION_DURATION);
    delay(ACCLIMATION_DURATION*1000);

    // START TRIALS
    for (int x = 0; x < N_TRIALS; x++) {

      
      // DETECT POSITION, DELIVER CS AND US        
      while (true) {

        RIGHT_ACTIVE = digitalRead(pir_r_pin);
        LEFT_ACTIVE = digitalRead(pir_l_pin);

        unsigned long START_TONE = millis();
        unsigned long CURRENT_TONE_DELAY = millis();
        if (RIGHT_ACTIVE){

          // DELIVER TONE IN THE RIGHT COMPARTMENT
          digitalWrite(speaker_r_pin, HIGH);
          Serial.println("CS_R > ON");

          // WHILE THE OPPOSITE COMPARTMENT IS NOT ACTIVE, CONTINUE FOR TONE_DURATION
          while (LEFT_ACTIVE != HIGH) {

            // CHECK IF LEFT IS ACTIVE
            LEFT_ACTIVE = digitalRead(pir_l_pin);

            // AFTER SPECIFIC DELAY, TRIGGER US
            if ((START_TONE - CURRENT_TONE_DELAY) > (TONE_DURATION * 1000)) {

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
              digitalWrite(speaker_r_pin, LOW);
              Serial.println("CS_R > OFF").
              break;
            }
          }
          
          // DELIVER TONE IN THE RIGHT COMPARTMENT
          digitalWrite(speaker_r_pin, HIGH);
          Serial.println("CS_R > ON");
          

          
        } else {

          // DELIVER TONE IN THE LEFT COMPARTMENT


          
          
        }

        
      }
      
      // INITIATE INTER-TRIAL-INTERVAL 

      
    }


    
  }
  









 

}
