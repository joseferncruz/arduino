
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

const int acclimation_seconds = 10 * 60L; // ACCLIMATION TIME IN SECONDS
const int cooldown_seconds = 10 * 60L;    // COOLDOWN TIME IN SECONDS

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

  // READ TEST SWITCH
  switchstate = digitalRead(2);
  
  if (switchstate == HIGH) { 
    
    // SIGNAL START OF EXPERIMENT WITH BLINKING LED 
    Serial.println("NEW EXPERIMENT: CLASSICAL THREAT CONDITIONING");
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

    Serial.print("ACCLIMATION TIME (SEC): ");
    Serial.println(acclimation_seconds);
    delay(500);    

    Serial.print("COOLDOWN TIME (SEC): ");
    Serial.println(cooldown_seconds);
    delay(500);


    /*###############################################################################*/

    Serial.println("NEW EXPERIMENT: START");
    delay(1000);

    Serial.print("ACCLIMATION (SEC): "); Serial.println(acclimation_seconds);
    
    delay(acclimation_seconds*1000L);

    // INITIATE COOLDDOWN AT THE END OF EXPERIMENT
    Serial.print("COOLDOWN (SEC): "); Serial.println(cooldown_seconds);
  
    Serial.println("NEW EXPERIMENT: END");
    Serial.println("PRESS ARDUINO RESET BUTTON TO RESTART A NEW EXPERIMENT");
  }
}
