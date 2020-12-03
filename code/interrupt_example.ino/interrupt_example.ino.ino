#include <Arduino.h>
// Uses https://github.com/PaulStoffregen/TimerOne for sending on a regular interval
#include <TimerOne.h>

// ECHO pin, needs to be a pin that supports interrupts!
#define ULTRASONIC_PIN_INPUT      2
// TRIG pin, can be any output pin
#define ULTRASONIC_PIN_OUTPUT     13
// update interval, make sure to keep it above 20ms
#define ULTRASONIC_TIMER_US       50000 

// the time the pulse was sent
volatile long ultrasonic_echo_start = 0;
// the distance once it's been measured
volatile long ultrasonic_distance = 0;

void ultrasonicPulse(){
    // Sets the trigger on HIGH state for 10 micro seconds to send a series of pulses
    digitalWrite(ULTRASONIC_PIN_OUTPUT, HIGH);
    // blocks 10 microseconds from the interrupt, I think we'll live :)
    delayMicroseconds(10);
    // disable the sending again so we can wait for a response
    digitalWrite(ULTRASONIC_PIN_OUTPUT, LOW);
    // record the send time
    ultrasonic_echo_start = micros();
}

void ultrasonicEcho(){
    // don't do anything if no pulse has been sent
    if(ultrasonic_echo_start != 0){
        // calculate the distance by measuring how long it took to return the sound
        // The speed of sound is 343 m/s and we need half the time it took (since
        // the sound has to travel towards the object _AND_ back). So a single echo does
        // 1/(343/2) = 0.005831 seconds per meter
        ultrasonic_distance = (micros() - ultrasonic_echo_start) / 58;
        ultrasonic_echo_start = 0;
    }
}

void setup(){
    Serial.begin(115200);
  
    // set the echo pin to receive interrupts, on an arduino uno only pin 2 and 3 can link to interrupts
    pinMode(ULTRASONIC_PIN_INPUT, INPUT_PULLUP);
    pinMode(ULTRASONIC_PIN_OUTPUT, OUTPUT);
  
    // set the update interval for sending the trigger
    Timer1.initialize(ULTRASONIC_TIMER_US);
    // link the trigger function to the timer
    Timer1.attachInterrupt(ultrasonicPulse);
    // link the echo function to the echo pin
    attachInterrupt(digitalPinToInterrupt(ULTRASONIC_PIN_INPUT), ultrasonicEcho, RISING);
}

void loop() {
  Serial.print("Distance: ");
  Serial.println(ultrasonic_distance);
  delay(ULTRASONIC_TIMER_US / 1000);
}
