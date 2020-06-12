
// get some packages


// set up some constants and variables

int cs_len = 5; // number of seconds per cs
int us_len = 2; // number of seconds per us
int switchstate = 0; // Button starts the experiment

int detect_cs = 0; // detect if the cs was delivered.
int detect_us = 0; // detect if the us was delivered


// list of the inter-trial-intervals: ITI
int itintervals[] = {6, 9, 12, 16, 18};


void setup() {
  // Start serial
  Serial.begin(9600); // serial will be read by bonsai
  Serial.println("LeDoux Lab: Classical Threat Conditioning Procedure");
  
  // Digital Output pins
  pinMode(7, OUTPUT); // Delivering the CS
  pinMode(8, OUTPUT); // Delivering the US
  pinMode(9, OUTPUT); // Delivering the CHAMBER LED when the CS is delivered.
  pinMode(11, OUTPUT); // Signal the deliver of CS
  pinMode(12, OUTPUT); // Signal the deliver of the US

  // Button to start the experiment.
  pinMode(2, INPUT); // single puch to start
  
}

void loop() {
  
  int total_cs_number = 5; // How many CS is going to be delivered.
  
  // variable to keep track of the number of CS at a give time
  int current_cs = 1;
  
  switchstate = digitalRead(2);

  // Only start if the switch is pressed
  if (switchstate == HIGH) { 
    
    // Signal the start
    Serial.println("START");

    // Test the LEDs
    Serial.println("Testing LEDs");
    delay(10*1000); // 10 seconds

    
    Serial.println("Acclimation: 5 seconds");
    delay(5 * 1000);
    
    while (total_cs_number > 0) {

      Serial.print("CS: 0");
      Serial.println(current_cs);
      
      // CS --> ON
      digitalWrite(7, HIGH);
      digitalWrite(9, HIGH); // Deliver Chamber LED
      digitalWrite(11, HIGH);
      
      // 
      delay( (cs_len - us_len) * 1000);
      
      // US --> ON
      digitalWrite(8, HIGH);
      digitalWrite(12, HIGH);
      Serial.println("Delivering US");

      // US/US --> Paired
      delay(us_len * 1000); // Transform in seconds
    
      // US/CS --> OFF
      for (int i = 7; i < 9; i = i + 1) {
        digitalWrite(i, LOW);
      }
      for (int i = 11; i < 13; i = i + 1) {
        digitalWrite(i, LOW);
      }
      digitalWrite(9, LOW);
    
      // Wait one second before restarting
      delay(1000);

      // Calculate the remaining CS
      total_cs_number = total_cs_number - 1;
      
      // Current cs
      current_cs = current_cs + 1;

      
      // Display the inter-trial-interval 
      int delay_iti = itintervals[random(0, 5)];
      
      Serial.print("Inter-Trial-Interval (sec): ");
      Serial.println(delay_iti);
      
      delay(delay_iti * 1000); // Transform the delay into seconds
      
    }

  // Cooldown before finishing the experiment")
  Serial.println("Cooldown: 10 seconds");
  delay(10 * 1000);
  Serial.println("END");
  }
}
