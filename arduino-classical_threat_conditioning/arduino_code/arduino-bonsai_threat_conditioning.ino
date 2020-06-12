
// get some packages


// set up some constants and variables

int cs_len = 5;
int us_len = 2; 
int switchstate = 0; // Button starts the experiment

int itintervals[] = {6, 9, 12, 16, 18};


void setup() {
  // Start serial
  Serial.begin(9600);
  Serial.println("Start Threat Conditioning Sketch ");
  
  // Digital Output pins
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

  // Digital Input pins
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // Start button
  pinMode(5, INPUT);
  
}

void loop() {
  

  int total_cs_number = 5;
  int current_cs = 0;

  switchstate = digitalRead(5);
  
  if (switchstate == HIGH) { 
    Serial.println("Experiment START");
    
    // Only start if the switch is pressed
    while (total_cs_number > 0) {

      Serial.print("Delivering CS number: ");
      Serial.println(current_cs);
      
      // CS ON
      digitalWrite(7, HIGH);
      // read value from digital read
      
      // wait with the cs ON
      delay( (cs_len - us_len) * 1000);
      
      // us ON
      digitalWrite(8, HIGH);
      Serial.println("Delivering US");

      
      // allow pairing of the 2 signals
      delay(us_len * 1000);
    
      // Turn CS and US OFF
      for (int i = 7; i < 9; i = i + 1) {
        digitalWrite(i, LOW);
      }
    
      // Wait one second before restarting
      delay(1000);

      // subtract the current round from the cs_number
      total_cs_number = total_cs_number - 1;
      // Current cs
      current_cs = current_cs + 1;

      Serial.print("Number of CS missing: ");
      Serial.println(total_cs_number); 

      
      // Display the inter-trial-interval 
      int delay_iti = itintervals[random(0, 5)];
      Serial.print("Inter-Trial-Interval (sec): ");
      Serial.println(delay_iti);
      delay(delay_iti * 1000);
      
    }
  Serial.println("Experiment END");
  }
}
