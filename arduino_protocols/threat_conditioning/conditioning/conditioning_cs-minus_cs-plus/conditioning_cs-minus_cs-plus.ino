
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

const int acclimation_seconds = 5 * 60L; // ACCLIMATION TIME IN SECONDS
const int cooldown_seconds = 5 * 60L;    // COOLDOWN TIME IN SECONDS
const int cs_plus_len = 30;              // DURATION CS-PLUS
const int cs_minus_len = 30;             // DURATION CS-MINUS
const int us_len = 2;                    // DURATION US


// VARIABLES
/*###############################################################################*/

int total_cs_plus_number = 5;            // NUMBER OF CS-PLUS
int total_cs_minus_number = 5;           // NUMBER OF CS-MINUS

int switchstate = 0;                     // Button starts the experiment
int switchstate_test_chamber = 0;        // TEST CHAMBER LED
int detect_cs = 0;                       // detect if the cs was delivered.
int detect_us = 0;                       // detect if the us was delivered


// LIST OF INTER-TRIAL-INTERVALS (ITI) IN SECONDS
/*###############################################################################*/

int itintervals[] = {60, 90, 120, 160, 180};




void setup() {
  
  // INITIATE SERIAL
  Serial.begin(9600);                    // SERIAL IS READ BY BONSAI
  delay(5 * 1000);
  Serial.println("LEDOUX LAB");
  
  // DIGITAL OUTPUTS
  pinMode(6, OUTPUT);                    // DELIVER CS-MINUS
  pinMode(7, OUTPUT);                    // DELIVER CS-PLUS
  pinMode(8, OUTPUT);                    // DELIVER US
  pinMode(9, OUTPUT);                    // CHAMBER LED DURING CS-MINUS AND CS-PLUS
  pinMode(11, OUTPUT);                   // BOARD LED TO SIGNAL CS-MINUS AND CS-PLUS
  pinMode(12, OUTPUT);                   // BOARD LED TO SIGNAL US

  // INPUTS TO START EXPERIMENT
  pinMode(2, INPUT);                     // INPUT SWITCH TO START EXPERIMENT
  pinMode(3, INPUT);                     // INPUT SWITCH TO TEST CHAMBER LED, CS-MINUS, CS-PLUS, US
  
}

void loop() {

    /* TEST CHAMBER LED, CS-MINUS, CS-PLUS, US
    ###############################################################*/
    
    switchstate_test_chamber = digitalRead(3);
  
    if (switchstate_test_chamber == HIGH) {
  
      // TEST CHAMBER LED, US TTL AND CS TTL
      Serial.println("TEST CHAMBER LED, CS-MINUS, CS-PLUS & US");
      
      // TEST CHAMBER LED
      Serial.println("CHAMBER LED: ON");
      digitalWrite(9, HIGH);
      delay(5000);
      digitalWrite(9, LOW);
      Serial.println("CHAMBER LED: OFF");
      
      delay(1000);
  
      // TEST CS-MINUS
      Serial.println("CS-MINUS: ON");
      digitalWrite(6, HIGH);
      delay(5000);
      digitalWrite(6, LOW);
      Serial.println("CS-MINUS: OFF");
  
      delay(1000);

      // TEST CS-PLUS
      Serial.println("CS-PLUS: ON");
      digitalWrite(7, HIGH);
      delay(5000);
      digitalWrite(7, LOW);
      Serial.println("CS-PLUS: OFF");
  
      delay(1000);
  
      // TEST US
      Serial.println("US: ON");
      digitalWrite(8, HIGH);
      delay(2000);
      digitalWrite(8, LOW);
      Serial.println("US: OFF");

      
    }
    /*###################################################################*/


  /* START NEW EXPERIMENT
  #######################################################################*/  
  
  // VARIABLE USED TO KEEP TRACK OF THE CURRENT CS-PLUS NUMBER
  int current_cs_plus = 1;

  // READ TEST SWITCH
  switchstate = digitalRead(2);
  
  if (switchstate == HIGH) { 
    
    // TEST CHAMBER LEDs
    Serial.println("TEST CHAMBER LED");
    Serial.println("LED: ON");
    digitalWrite(9, HIGH);
    delay(10*1000); // 10 seconds
    digitalWrite(9, LOW);
    Serial.println("LED: OFF");
    delay(5 * 1000);

    // PRINT INFORMATION ABOUT EXPERIMENT
    
    Serial.println("NEW EXPERIMENT: CLASSICAL THREAT CONDITIONING");
    delay(1000);
    Serial.println("SESSION: CONDITIONING");
    delay(1000);
    
    Serial.print("NUMBER OF CS-MINUS: ");
    Serial.println(total_cs_minus_number);
    delay(500);

    Serial.print("NUMBER OF CS-PLUS: ");
    Serial.println(total_cs_plus_number);
    delay(500);

    Serial.print("CS DURATION (SEC): ");
    Serial.println(cs_plus_len);
    delay(500);
    
    Serial.print("NUMBER OF US: ");
    Serial.println(total_cs_plus_number);        // SAME NUMBER OF CS-PLUS
    delay(500);

    Serial.print("US DURATION (SEC): ");
    Serial.println(us_len);
    delay(500);

    Serial.print("US START AT (CS DURATION - US DURATION): ");
    Serial.println((cs_plus_len - us_len));
    delay(500);

    Serial.print("ACCLIMATION TIME (SEC): ");
    Serial.println(acclimation_seconds);
    delay(500);    

    Serial.print("COOLDOWN TIME (SEC): ");
    Serial.println(cooldown_seconds);
    delay(500);


    /*#####################################################################*/

    Serial.println("NEW EXPERIMENT: START");
    delay(1000);

    Serial.print("ACCLIMATION (SEC): ");
    Serial.println(acclimation_seconds);
    delay(acclimation_seconds*1000L);

    // EXPERIMENT LOOP
    while (total_cs_plus_number > 0) {

      Serial.print("CS-PLUS: 0");
      Serial.println(current_cs_plus);
      
      // CS-PLUS --> ON
      digitalWrite(7, HIGH);
      Serial.print("CS-PLUS: ON");
      
      digitalWrite(9, HIGH);                     // CHAMBER LED: ON
      digitalWrite(11, HIGH);                    // ARDUINO LED: ON 
      
      // FROM START OF CS-PLUS TO START OF US
      delay( (cs_plus_len - us_len) * 1000);
      
      // US --> ON
      digitalWrite(8, HIGH);
      digitalWrite(12, HIGH);                    // ARDUINO LED: ON 
      Serial.println("US: ON");

      // PAIR US WITH CS-PLUS
      delay(us_len * 1000);                      // IN SECONDS
    
      // US/CS --> OFF
      for (int i = 7; i < 9; i = i + 1) {
        digitalWrite(i, LOW);
      }
      Serial.println("US: OFF");
      Serial.println("CS-PLUS: OFF");
      
      for (int i = 11; i < 13; i = i + 1) {
        digitalWrite(i, LOW);
      }
      digitalWrite(9, LOW);
    
      // WAIT ONE SECOND
      delay(1000);






      // CALCULATE REMAINING CS
      total_cs_plus_number = total_cs_plus_number - 1;
      
      // CURRENT CS NUMBER
      current_cs_plus = current_cs_plus + 1;

      // PRINT INTER-TRIAL-INTERVAL 
      int delay_iti = itintervals[random(0, 5)];
      
      Serial.print("INTER-TRIAL-INTERVAL (SEC): ");
      Serial.println(delay_iti);
      
      delay(delay_iti*1000L); // Transform the delay into seconds
      
    }

  // INITIATE COOLDDOWN AT THE END OF EXPERIMENT
  Serial.print("COOLDOWN (SEC): ");
  Serial.println(cooldown_seconds);
  delay(cooldown_seconds*1000L);

 
  Serial.println("NEW EXPERIMENT: END");
  }
}
