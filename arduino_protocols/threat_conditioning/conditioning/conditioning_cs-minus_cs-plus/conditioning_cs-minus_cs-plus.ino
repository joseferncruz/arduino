
/* --------------------------------------------------------------------------------
 * LeDoux Lab 2021
 * 
 * Jose Oliveira da Cruz 
 * jose.cruz@nyu.edu
 * 
 * --------------------------------------------------------------------------------
 */


// USER INPUT VARIABLES
/*###############################################################################*/

const int ACCLIMATION_TIME_SEC = 5 * 60L; // ACCLIMATION TIME IN SECONDS
const int COOLDOWN_TIME_SEC = 5 * 60L;    // COOLDOWN TIME IN SECONDS

int TOTAL_CS_PLUS = 5;                    // NUMBER OF CS-PLUS
const int cs_plus_len = 30;                // DURATION CS-PLUS
const int us_len = 1;                    // DURATION US


int TOTAL_CS_MINUS = 0;                   // NUMBER OF CS-MINUS  
const int cs_minus_len = 0;             // DURATION CS-MINUS


/* IMPORTANT: IF CS-PLUS > 0 AND current_cs_type != 1, THE LOOP WILL NOT WORK
              OR IF CS-minus > 0 AND current_cs_type != 0, THE LOOP WILL ALSO NOT WORK*/
int current_cs_type = 1;                    // STARTING CS: 0 == CS-MINUS | 1 == CS-PLUS


// LIST OF INTER-TRIAL-INTERVALS (ITI) IN SECONDS
/*###############################################################################*/
int itintervals[] = {60, 90, 120, 160, 180};


// OTHER
/*###############################################################################*/
int switchstate = 0;                     // Button starts the experiment
int switchstate_test_chamber = 0;        // TEST CHAMBER LED
int detect_cs = 0;                       // detect if the cs was delivered.
int detect_us = 0;                       // detect if the us was delivered

int total_cs_plus_number = TOTAL_CS_PLUS;    
int total_cs_minus_number = TOTAL_CS_MINUS;      


unsigned long SESSION_START_TIME;
unsigned long SESSION_END_TIME;
/*###############################################################################*/


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
    /*###############################################################################*/
    
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


  /* START NEW EXPERIMENT
  /*###############################################################################*/ 
  
  // VARIABLE USED TO KEEP TRACK OF THE CURRENT CS-PLUS NUMBER
  int current_cs_plus = 1;
  int current_cs_minus = 1;

  // READ TEST SWITCH
  switchstate = digitalRead(2);
  
  if (switchstate == HIGH) { 
    
    // SIGNAL START OF EXPERIMENT WITH BLINKING LED 
    Serial.println("CLASSICAL THREAT CONDITIONING");
    delay(1000);
    for (int i = 0; i < 6; i++) {
      digitalWrite(9, HIGH);
      delay(500); 
      digitalWrite(9, LOW);
      delay(500);
      
    }

    // PRINT INFORMATION ABOUT EXPERIMENT
    /*###############################################################################*/
    Serial.println("SESSION: CONDITIONING");
    Serial.print("NUMBER OF CS-MINUS: "); Serial.println(total_cs_minus_number);
    Serial.print("NUMBER OF CS-PLUS: "); Serial.println(total_cs_plus_number);
    Serial.print("CS DURATION (SEC): "); Serial.println(cs_plus_len);
    Serial.print("NUMBER OF US: "); Serial.println(total_cs_plus_number);        // SAME NUMBER OF CS-PLUS
    Serial.print("US DURATION (SEC): "); Serial.println(us_len);
    Serial.print("US START AT (CS DURATION - US DURATION): "); Serial.println((cs_plus_len - us_len));
    Serial.print("ACCLIMATION TIME (SEC): "); Serial.println(ACCLIMATION_TIME_SEC);
    Serial.print("COOLDOWN TIME (SEC): "); Serial.println(COOLDOWN_TIME_SEC);
    /*###############################################################################*/
    Serial.println("SESSION > START");

    Serial.println("ACCLIMATION > START");
    delay(ACCLIMATION_TIME_SEC*1000L);
    Serial.println("ACCLIMATION > END");
    Serial.println("CONDITIONING > START");
    SESSION_START_TIME = millis();
    
    // LOOP UNTIL THERE ARE NO MORE CS-MINUS AND CS-PLUS AVAILABLE. ALTERNATE BETWEEN THEM
    while (total_cs_plus_number > 0 || total_cs_minus_number > 0) {

      // DEFINE STARTING CS
      /*###############################################################################*/
      if (current_cs_type == 1 & total_cs_plus_number > 0) {
        // RUN CS-PLUS

        // DISPLAY CURRRENT CS NUMBER
        Serial.print("CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.println(" > ON");
        
        // CS-PLUS --> ON
        digitalWrite(7, HIGH);
        digitalWrite(9, HIGH);                     // CHAMBER LED: ON
        digitalWrite(11, HIGH);                    // ARDUINO LED: ON 
        
        // FROM CS-PLUS ONSET TO US ONSET
        delay( (cs_plus_len - us_len) * 1000);
        
        // US --> ON
        digitalWrite(8, HIGH);
        digitalWrite(12, HIGH);                    // ARDUINO LED: ON 
        Serial.println("US > ON");
  
        // PAIR US WITH CS-PLUS
        delay(us_len * 1000);                      // IN SECONDS
      
        // US/CS --> OFF
        for (int i = 7; i < 9; i = i + 1) {
          digitalWrite(i, LOW);
        }
        Serial.println("US > OFF");
        Serial.print("CS-PLUS: 0"); Serial.print(current_cs_plus); Serial.println(" > OFF");
        
        for (int i = 11; i < 13; i = i + 1) {
          digitalWrite(i, LOW);
        }
        digitalWrite(9, LOW);

        // CALCULATE REMAINING CS-PLUS
        total_cs_plus_number = total_cs_plus_number - 1;
        
        // CALCULATE CURRENT CS-PLUS
        current_cs_plus = current_cs_plus + 1;
  
        // PRINT INTER-TRIAL-INTERVAL 
        int delay_iti = itintervals[random(0, 5)];
        Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(delay_iti);

        // INITIATE ITI
        delay(delay_iti*1000L); 

        // SWITCH TO CS-MINUS IF THERE ARE CS-MINUS AVAILABLE
        if (current_cs_type == 1 & total_cs_minus_number > 0) {
          
          current_cs_type = 0;
          
          }

        
      }
      /*###############################################################################*/
      else if (current_cs_type == 0 && total_cs_minus_number > 0) {
        // RUN CS-MINUS

        // DISPLAY CURRRENT CS NUMBER
        Serial.print("CS-MINUS: 0"); Serial.print(current_cs_minus); Serial.println(" > ON");
        
        // CS-MINUS --> ON
        digitalWrite(6, HIGH);
        digitalWrite(9, HIGH);                     // CHAMBER LED: ON
        digitalWrite(11, HIGH);                    // ARDUINO LED: ON 
        
        // CS-MINUS DURATION
        delay(cs_minus_len * 1000L);
        

        // CS-MINUS OFF 
        digitalWrite(6, LOW);                     // TURN CS-MINUS OFF
        Serial.print("CS-MINUS: 0"); Serial.print(current_cs_minus); Serial.println(" > OFF");
        
        digitalWrite(9, LOW);                     // CHAMBER LED: ON
        digitalWrite(11, LOW);                    // ARDUINO LED: ON         
        
        // WAIT ONE SECOND
        delay(1000);

        // CALCULATE REMAINING CS-MINUS
        total_cs_minus_number = total_cs_minus_number - 1;
        
        // CALCULATE CURRENT CS-MINUS
        current_cs_minus = current_cs_minus + 1;
  
        // PRINT INTER-TRIAL-INTERVAL 
        int delay_iti = itintervals[random(0, 5)];
        Serial.print("INTER-TRIAL-INTERVAL (SEC): "); Serial.println(delay_iti);

        // INITIATE ITI
        delay(delay_iti*1000L); 

        // SWITCH TO CS-MINUS IF THERE ARE CS-MINUS AVAILABLE
        if (current_cs_type == 0 && total_cs_plus_number > 0) {
          
          current_cs_type = 1;
          
          }

      } 
      /*###############################################################################*/
      else {
        // DO NOTHING

        }   
      
    }
    

  // INITIATE COOLDDOWN AT THE END OF EXPERIMENT
  Serial.println("CONDITIONING > END");
  SESSION_END_TIME = millis();
  unsigned long SESSION_DURATION_DELTA = (SESSION_END_TIME - SESSION_START_TIME) / 1000L;
  
  Serial.println("COOLDOWN > START");
  delay(COOLDOWN_TIME_SEC*1000L);
  Serial.println("COOLDOWN > END");
  
  Serial.println("SESSION > END");
  Serial.print("SESSION DURATION (SEC): "); Serial.println(SESSION_DURATION_DELTA+ACCLIMATION_TIME_SEC+COOLDOWN_TIME_SEC);
  
  } else {
    // RESET VARIABLES
    total_cs_plus_number = TOTAL_CS_PLUS;    
    total_cs_minus_number = TOTAL_CS_MINUS;    
    
  }

  



  
}
