
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, josecruz.cbr@gmail.com 





 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>



const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

const int food_tray_led = 12;

const int lever_press = 7;

int clockwise = 0;

int lever_state = 0;
int press_lapse = 0;

int counting_presses = 0;

// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(80);
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println("LEVER PRESS TRAINING PROTOCOL");

  // PINs
  pinMode(food_tray_led, OUTPUT);
  pinMode(lever_press, INPUT);
  
  
}

void loop() {
  
  // DETECT LEVER STATE
  lever_state = digitalRead(lever_press);

  // IF LEVER ON AND IF IT IS THE FIRST TIME IT HAS BEEN PRESSED
  if ((lever_state == HIGH) && (press_lapse == 0)) {
    Serial.println("--------------------------------------");
    press_lapse = 1;
    Serial.println("LEVER ON");

    if (clockwise == 0) {  
      Serial.println("REWARD");
      digitalWrite(food_tray_led, HIGH);
      myStepper.step(stepsPerRevolution);
      delay(6*1000L);
      digitalWrite(food_tray_led, LOW);
      delay(2000);
      counting_presses = 0; 
      clockwise = 1;
    } else if (clockwise == 1) {
      Serial.println("REWARD");
      digitalWrite(food_tray_led, HIGH);
      myStepper.step(-stepsPerRevolution);
      delay(6*1000L);
      digitalWrite(food_tray_led, LOW);
      delay(2000);
      counting_presses = 0; 
      clockwise = 0;
    }

    
    delay(100);
    counting_presses ++;
  } 
  // IF LEVER ON AND NOT THE FIRST TIME IT HAS BEEN PRESSED
  else if ((lever_state == HIGH) && (press_lapse == 1)){
    // DO NOTHING
  } 

  // WHEN LEVER IS OFF, RESET THE RECORD
  else if ((lever_state == LOW) && (press_lapse == 1)) {
    press_lapse = 0;
    Serial.println("LEVER OFF");
    delay(100);
  }

  // ELSE DO NOTHING
  else {
  }

/*
 
  // TRIGGER THE REWARD
  if (counting_presses >= 30) {
    Serial.println("REWARD");
    digitalWrite(food_tray_led, HIGH);
    myStepper.step(stepsPerRevolution);
    delay(15*1000L);
    digitalWrite(food_tray_led, LOW);
    delay(2000);
    counting_presses = 0;    
  }

*/
}
