
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, josecruz.cbr@gmail.com 





 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>



const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

const int food_tray_led = 12;

const int lever_press = 7;

int detect_level_press = 0;
int press_lapse = 0;

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
  
  // Detect first lever press
  detect_level_press = digitalRead(lever_press);
  //Serial.println(detect_level_press);
  if ((detect_level_press == HIGH) && (press_lapse == 0)) {
    Serial.println("--------------------------------------");
    press_lapse = 1;
    Serial.println("LEVER ON");
    delay(100);
  } else if ((detect_level_press == HIGH) && (press_lapse == 1)){
    
  } else if ((detect_level_press == LOW) && (press_lapse == 1)) {
    press_lapse = 0;
    Serial.println("LEVER OFF");
    delay(100);
  }
  else {
  }
  
  
/*
  if ((detect_level_press > 0) && (press_lapse == 0)) {
    Serial.println("lever pressed");
    Serial.println(press_lapse);
    press_lapse = 1;
    delay(200);
  }
  else if ((detect_level_press > 0) && (press_lapse == 0)) {
    delay(15);
    
  }
  else {
    press_lapse = 0;
    delay(15);
  }
 */ 
  
  // step one revolution  in one direction:
  //Serial.println("deliver reward");
  //digitalWrite(food_tray_led, HIGH);
  //myStepper.step(stepsPerRevolution);
  //delay(15*1000L);
  //digitalWrite(food_tray_led, LOW);
  //delay(2000);
}
