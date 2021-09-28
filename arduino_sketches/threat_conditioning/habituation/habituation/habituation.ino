
/* --------------------------------------------------------------------------------
 * LeDoux Lab 2020
 * 
 * Jose Oliveira da Cruz 
 * jose.cruz@nyu.edu
 * 
 * --------------------------------------------------------------------------------
 */

// Constants

const int acclimation_seconds = 5 * 60L; // IN SECONDS
const int cooldown_seconds = 5 * 60L;    // IN SECONDS
const int habituation_seconds = 20 * 60L; // IN SECONDS

// Variables

int switchstate = 0; // Button starts the experiment
int switchstate_test_led = 0; // Test the led
int detect_cs = 0; // detect if the cs was delivered.
int detect_us = 0; // detect if the us was delivered

// list of the inter-trial-intervals: ITI
int itintervals[] = {6, 9, 12, 16, 18};


void setup() {
  // Start serial
  Serial.begin(9600); // serial will be read by bonsai
  delay(5 * 1000);
  Serial.println("LEDOUX LAB");
  Serial.println("PRESS GREEN SWITCH TO START NEW EXP");
  
  // Digital Output pins
  pinMode(7, OUTPUT); // Delivering the CS
  pinMode(8, OUTPUT); // Delivering the US
  pinMode(9, OUTPUT); // Delivering the CHAMBER LED when the CS is delivered.
  pinMode(11, OUTPUT); // Signal the deliver of CS to LED
  pinMode(12, OUTPUT); // Signal the deliver of US to LED

  // Button to start the experiment.
  pinMode(2, INPUT); // single puch to start
  pinMode(3, INPUT); // button to test LEDs
  
}

void loop() {

  // TURN CHAMBER LED ON
  switchstate_test_led = digitalRead(3);

  if (switchstate_test_led == HIGH) {

    /*TEST CHAMBER LED, US TTL AND CS TTL*/
    Serial.println("TEST CHAMBER LED, CS & US");
    
    // TEST CHAMBER LED
    Serial.println("CHAMBER LED: ON");
    digitalWrite(9, HIGH);
    delay(5000);
    digitalWrite(9, LOW);
    Serial.println("CHAMBER LED: OFF");
    
    delay(1000);

    // TEST CS TTL
    Serial.println("CS: ON");
    digitalWrite(7, HIGH);
    delay(5000);
    digitalWrite(7, LOW);
    Serial.println("CS: OFF");

    delay(1000);

    // TEST US TTL
    Serial.println("US: ON");
    digitalWrite(8, HIGH);
    delay(2000);
    digitalWrite(8, LOW);
    Serial.println("US: OFF");
  }
  
  // variable to keep track of the number of CS at a give time
  int current_cs = 1;


  // START NEW EXPERIMENT
  switchstate = digitalRead(2);
  if (switchstate == HIGH) { 

    Serial.println("NEW EXPERIMENT: CLASSICAL THREAT CONDITIONING");
    delay(1000);
    Serial.println("SESSION: HABITUATION"); // SESSION NAME
    delay(1000);
    
    // TEST CHAMBER LEDs
    Serial.println("TEST CHAMBER LED");
    Serial.println("LED: ON");
    digitalWrite(9, HIGH);
    delay(10*1000); // 10 seconds
    digitalWrite(9, LOW);
    Serial.println("LED: OFF");
    delay(5 * 1000);

    //  PRINT INFORMATION ABOUT SESSION

    Serial.print("DURATION OF THE SESSION (sec): ");
    Serial.println(habituation_seconds);
    delay(500);


    // START THE EXPERIMENT
    
    Serial.println("NEW EXPERIMENT: START");
    delay(1000);

    Serial.print("ACCLIMATION (sec): ");
    Serial.println(acclimation_seconds);
    delay(acclimation_seconds * 1000L);


    // Delay over the HABITUATION PERIOD
    Serial.println("HABITUATION PERIOD: START");
    delay(habituation_seconds*1000L);
    Serial.println("HABITUATION PERIOD: START");

  
    // COOLDOWN AFTER EXPERIMENT CYCLE
    Serial.print("COOLDOWN (sec): ");
    Serial.println(cooldown_seconds);
    delay(cooldown_seconds * 1000L);

 
  Serial.println("NEW EXPERIMENT: END");
  }
}
