//  Box Lab 5
//
// If we see a voltage change on pin 2 the toggle switch on top of the useless box has 
// changed position and we need to react!
//    A HIGH value on pin 2 means we should activate the servo to open the useless 
//    box and attempt to return the switch to the "off" position.
//    A LOW value on pin 2 means the switch is off and we should return to our 
//    inital (closed box) state.

#include <Servo.h> 

#define servoPin  10

Servo servo;

void setup()
{
  Serial.begin(9600);
  // connect to our servo and make sure it is in the closed position
  servo.attach(servoPin);
  servo.write(40);
}

int speed = 0;
int diff = 30;
void loop()
{ 
//  servo.write(diff+60);
//  diff = (diff + 10)% 
  delay(200);
}
