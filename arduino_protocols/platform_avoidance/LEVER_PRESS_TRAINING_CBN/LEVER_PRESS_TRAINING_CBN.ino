
/*

LeDoux Lab | 2020
Jose Oliveira da Cruz, jose.cruz@nyu.edu


LEVER_PRESS_TRAINING_CBN
----------------------
This protocols is build to the subject to lever press in order to get the food in the Coulbourn Chambers. 


 */

// Library used to control the stepper associated with the food magazine.
#include <Stepper.h>


const int stepsPerRevolution = 200;  // steps per revolution
const int food_tray_led = 9;        // LED in the chamber food tray 

const int lever_press = 8;           // LEVER_PRESS DECTECTOR


int lever_state = 0;
int press_lapse = 0;
int counting_presses = 0;



// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 10, 11, 12, 13);

void setup() {
  myStepper.setSpeed(80); // Speed of the step.
  // initialize the serial port:
  Serial.begin(9600);
  Serial.println("LEVER_PRESS_TRAINING");
  Serial.println("FR: 01");

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

      // TRIGGER FOOD DISPENSER 
      Serial.println("FOOD");
      digitalWrite(food_tray_led, HIGH);

      // MOVE HALF OF THE STEPPER REVOLUTIONS
      myStepper.step(stepsPerRevolution/2);

      // WAIT TWO SECOND
      delay(2*1000L);

      digitalWrite(food_tray_led, LOW);
      delay(2000);
      counting_presses = 0;   
    delay(100);

    // KEEP TRACK OF THE NUMBER OF LEVER PRESSES
    counting_presses ++;
  } 
  /* 
   *  IF LEVER ON AND NOT THE FIRST TIME IT HAS BEEN PRESSED
   *  THIS MEANS THAT THE LEVER IS STILL ON
  */
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

}
