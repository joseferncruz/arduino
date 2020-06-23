
const int switchPin = 2;
const int motorPin = 9;
int switchState = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(motorPin, OUTPUT);
  pinMode(switchPin, INPUT);
}

void loop() {

  switchState = digitalRead(switchPin);
  // put your main code here, to run repeatedly:
  if (switchState == HIGH) {
    Serial.println(switchState);
    digitalWrite(motorPin, HIGH);
  }
  else {
    digitalWrite(motorPin, LOW);
  }
}
