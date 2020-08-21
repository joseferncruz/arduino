
/* --------------------------------------------------------------------------------
 * LeDoux Lab 2020
 * 
 * Jose Oliveira da Cruz 
 * jose.cruz@nyu.edu
 * 
 * 
 * PLATFORM AVOIDANCE THREAT PROTOCOL - CODE FOR ARDUINO02 - ARD02
 * 
 * 
 * 
 * --------------------------------------------------------------------------------
 */

// SETTINGS OF CS/US/SESSIONS

const int cs_len = 30;             // DURATION CS
const int us_len = 2;              // DURATION US
int total_cs_number = 5;           // NUMBER OF CS 

int itintervals[] = {60, 90, 120, 160, 180}; // list of the inter-trial-intervals: ITI

// ACCLIMATION AND COOLDOWN

const int acclimation_seconds = 3 * 60L; // IN SECONDS
const int cooldown_seconds = 3 * 60L;    // IN SECONDS


// OTHERS

int switchstate = 0; // Button starts the experiment
int switchstate_test_led = 0; // Test the led
int detect_cs = 0; // detect if the cs was delivered.
int detect_us = 0; // detect if the us was delivered


void setup() {
  // Start serial
  Serial.begin(9600); 
  
  // PRINT INITIAL INFORMATION
  Serial.println("----------------------------------");
  Serial.println("LEDOUX LAB");
  Serial.println("PLATFORM AVOIDANCE");
  Serial.println("----------------------------------");
  Serial.print("CS LEN (SEC): "); Serial.println(cs_len);
  Serial.print("US LEN (SEC): "); Serial.println(us_len);
  Serial.print("US STARTS AT "); Serial.print(cs_len - us_len); Serial.println("(SEC) AFTER CS ONSET");
  Serial.print("NUMBER OF CS-US PAIRS: "); Serial.println(total_cs_number);
  Serial.print("ACCLIMATION (SEC): "); Serial.print(acclimation_seconds); 
  Serial.print(" | COOLDOWN (SEC): "); Serial.println(cooldown_seconds);
  Serial.println("----------------------------------");
  Serial.println("PRESS BLUE BUTTON TO START");
  Serial.println("----------------------------------");
  
  // DIGITAL OUTPUT
  pinMode(7, OUTPUT);              // Delivering the CS
  pinMode(8, OUTPUT);              // Delivering the US
  pinMode(9, OUTPUT);              // Delivering the CHAMBER LED when the CS is delivered.
  pinMode(11, OUTPUT);             // Signal the deliver of CS to LED
  pinMode(12, OUTPUT);             // Signal the deliver of US to LED
  pinMode(13, OUTPUT);             // control arduino01
  

  // DIGITAL INPUT
  pinMode(2, INPUT);               // PUSH BUTTON TO START
  pinMode(3, INPUT);               // TEST LEDS
  
}

void loop() {


  // TEST CHAMBER LED
  switchstate_test_led = digitalRead(3);

  if (switchstate_test_led == HIGH) {
    Serial.println("CHAMBER LED: ON");
    digitalWrite(9, HIGH);
    digitalWrite(13, HIGH);
    
    delay(5000);
    
    digitalWrite(9, LOW);
    digitalWrite(13, LOW);
    Serial.println("CHAMBER LED: OFF");
  }
  
  
  //int total_cs_number = 5; // How many CS is going to be delivered.
  
  int current_cs = 1; // KEEP TRACK OF THE CURRENT CS


  // START NEW EXPERIMENT
  switchstate = digitalRead(2);
  if (switchstate == HIGH) { 


    // START THE EXPERIMENT
    
    Serial.println("SESSION: START");

    // SIGNAL ARDUINO 01 TO START OWN CODE
    Serial.println("START LEVER PRESS PROTOCOL ON ARDUINO 01");
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);

    // ACCLIMATION
    Serial.print("ACCLIMATION (SEC): "); Serial.println(acclimation_seconds);
    // WAIT UNTIL THE END OF THE ACCLIMATION
    delay(acclimation_seconds*1000L);

    
    // THREAT PROTOCOL 
    
    while (total_cs_number > 0) {

      Serial.print("CS: 0");
      Serial.println(current_cs);
      
      // CS --> ON
      digitalWrite(7, HIGH);
      digitalWrite(9, HIGH); // CHAMBER LED: ON
      digitalWrite(11, HIGH); // ARDUINO LED: ON 
      
      // CS-US interval
      delay( (cs_len - us_len) * 1000);
      
      // US --> ON
      digitalWrite(8, HIGH);
      digitalWrite(12, HIGH); // ARDUINO LED: ON 
      Serial.println("US: ON");

      // US/US --> Paired
      delay(us_len * 1000); // IN SECONDS
    
      // US/CS --> OFF
      for (int i = 7; i < 9; i = i + 1) {
        digitalWrite(i, LOW);
      }
      for (int i = 11; i < 13; i = i + 1) {
        digitalWrite(i, LOW);
      }
      digitalWrite(9, LOW);
    
      // WAIT ONE SECOND
      delay(1000);

      // CALCULATE REMAINING CS
      total_cs_number = total_cs_number - 1;
      
      // CURRENT CS NUMBER
      current_cs = current_cs + 1;

      // PRINT INTER-TRIAL-INTERVAL 
      int delay_iti = itintervals[random(0, 5)];
      
      Serial.print("INTER-TRIAL-INTERVAL (sec): ");
      Serial.println(delay_iti);
      
      delay(delay_iti*1000L); // Transform the delay into seconds
      
    }

  // COOLDOWN AFTER EXPERIMENT CYCLE
  Serial.print("COOLDOWN (sec): ");
  Serial.println(cooldown_seconds);
  delay(cooldown_seconds*1000L);

 
  Serial.println("NEW EXPERIMENT: END");
  }
}
