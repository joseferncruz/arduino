
/* --------------------------------------------------------------------------------
 * LeDoux Lab 2020
 * 
 * Jose Oliveira da Cruz 
 * jose.cruz@nyu.edu
 * 
 * --------------------------------------------------------------------------------
 */

// CONSTANTS
/*###############################################################################*/

const int SESSION_DURATION_TIME_SEC = 15 * 60L;    // SESSION DURATION IN SECONDS

const int ACCLIMATION_TIME_SEC = 0 * 60L; // ACCLIMATION_TIME_SEC TIME IN SECONDS
const int COOLDOWN_TIME_SEC = 0 * 60L;    // COOLDOWN_TIME_SEC TIME IN SECONDS

int switchstate = 0;                      // Button starts the experiment
int switchstate_test_chamber = 0;         // TEST CHAMBER LED


void setup() {
  
  // INITIATE SERIAL
  Serial.begin(9600);                     // SERIAL IS READ BY BONSAI
  delay(5 * 1000);
  Serial.println("LEDOUX LAB");

  //OUTPUT
  pinMode(9, OUTPUT);                     // CHAMBER LED

  // INPUTS TO START EXPERIMENT
  pinMode(2, INPUT);                      // INPUT SWITCH TO START EXPERIMENT
  pinMode(3, INPUT);                      // INPUT SWITCH TO TEST CHAMBER LED, CS-MINUS, CS-PLUS, US
  
}

void loop() {

    /* TEST CHAMBER LED, CS-MINUS AND CS-PLUS
    /*###############################################################################*/
    
    switchstate_test_chamber = digitalRead(3);
  
    if (switchstate_test_chamber == HIGH) {
  
      // TEST CHAMBER LED, US TTL AND CS TTL
      Serial.println("TEST CHAMBER LED");
      
      // TEST CHAMBER LED
      Serial.println("CHAMBER LED: ON");
      digitalWrite(9, HIGH);
      delay(5000);
      digitalWrite(9, LOW);
      Serial.println("CHAMBER LED: OFF");

      delay(1000);
      
    }
  /*###############################################################################*/


  /* START NEW EXPERIMENT
  /*###############################################################################*/ 

  // READ SWITCH
  switchstate = digitalRead(2);
  
  if (switchstate == HIGH) { 
    
    // SIGNAL START OF EXPERIMENT WITH BLINKING LED 
    Serial.println("CLASSICAL THREAT CONDITIONING");
    delay(1000);
    for (int i = 0; i < 5; i++) {
      digitalWrite(9, HIGH);
      delay(500); 
      digitalWrite(9, LOW);
      delay(500);
      
    }

    // PRINT INFORMATION ABOUT EXPERIMENT
    Serial.println("SESSION: HABITUATION");
    delay(1000);

    Serial.print("ACCLIMATION_TIME_SEC TIME (SEC): ");
    Serial.println(ACCLIMATION_TIME_SEC);
    delay(500);    

    Serial.print("HABITUATION TIME (SEC): ");
    Serial.println(SESSION_DURATION_TIME_SEC);
    delay(500);    

    Serial.print("COOLDOWN_TIME_SEC TIME (SEC): ");
    Serial.println(COOLDOWN_TIME_SEC);
    delay(500);


    /*###############################################################################*/
    Serial.println("SESSION > START");
    // INITIATE ACCLIMATION_TIME_SEC
    Serial.println("ACCLIMATION > START");
    delay(ACCLIMATION_TIME_SEC*1000L);
    Serial.println("ACCLIMATION > END");

    // INITIATE HABITUTATION
    Serial.println("HABITUATION > START");
    delay(SESSION_DURATION_TIME_SEC*1000L);
    Serial.println("HABITUATION > END");

    // INITIATE COOLDDOWN AT THE END OF EXPERIMENT
    Serial.println("COOLDOWN > START");
    delay(COOLDOWN_TIME_SEC*1000L);
    Serial.println("COOLDOWN > END");
  
    Serial.println("SESSION > END");
    Serial.print("TOTAL HABITUATION TIME (SEC): "); Serial.println(ACCLIMATION_TIME_SEC+COOLDOWN_TIME_SEC+SESSION_DURATION_TIME_SEC);
  }
}
