

// record 

const int cs_ttl = 8;
const int cs_det_pin = 2;

int cs_detector = 0;
int cs_trigger = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(cs_det_pin, INPUT);
  pinMode(cs_ttl, OUTPUT);
  pinMode(A0, OUTPUT);
}

void loop() {
  Serial.println("CS: ON");
  digitalWrite(cs_ttl, HIGH);
  
  cs_detector = analogRead(A0);
  Serial.println(cs_detector); // CS timestamp

  
  if (cs_detector > 0) {
    Serial.println("CS detected");
  }
  else {
    Serial.println("CS: NOT DETECTED");
  }

  
  delay(3000);
  digitalWrite(cs_ttl, LOW);
  Serial.println("CS: OFF");
  delay(2000);
}
