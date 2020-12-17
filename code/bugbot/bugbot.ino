#include <Servo.h>

#define servoPin  10
#define sendPin 11
#define trigPin 13
#define echoPin 2

const double PGAIN = 0.05;
const double IGAIN = 0;
const double DGAIN = 0.05;

Servo servo;
Servo sender;

void setup(){
  Serial.begin(9600);
  //---SERVO-----
  servo.attach(servoPin);
  servo.write(90); //stationary
  sender.attach(sendPin);
  sender.write(90);
  //---ULTRASONIC--
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT_PULLUP);
  cli(); //stop interrupts
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register
  OCR2A = 255;// = set max comparison value
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set bits for 1024 prescaler
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();
  attachInterrupt(INT0, detect_echo, CHANGE);
}

//variables for ultrasonic range detection
volatile unsigned long diff = 0;
volatile unsigned long distance = 0;
volatile unsigned long trig = 0;

//variables for PID control
int set_pos = 254/2; //set point centers face in camera
int err;
int err_prev = 0;
int err_diff;
int err_sum = 0;
int P, I, D;
int to_motor = 90;
int face_pos;
int msg;

unsigned long print_millis = 0;
void loop(){
  if((millis() - print_millis) > 500){
    if (distance >= 200 || distance <= 0){
      Serial.println("Out of range");
    }
    else {
      Serial.print(distance);
      Serial.println(" cm");
    }
    print_millis = millis()
  }
  if(Serial.available()){
    msg = Serial.read();
    if(msg == 255){ //255 means SEND IT
      sender.write(180);
      delay(500);
      sender.write(90);
    }
    else{
      face_pos = msg;
      //Serial.println(face_pos);
      if(face_pos != 254){ //254 means no face was detected
        err = set_pos - face_pos; //error from desired position
        err_diff = err_prev - err;
        err_sum += err;
        err_sum = constrain(err_sum, -100, 100);
        err_prev = err;
      }
      else{ //if face not seen, stop moving
        err = 0;
        err_diff = 0;
        err_sum = 0;
        err_diff = 0;
        //Serial.println("No Face Detcted");
      }
      if(abs(err) < 10){ //if close enough, stop moving
        err_diff = 0;
        err_sum = 0;
        err_diff = 0;
      }
  //    if(err > 0){
  //      P = 2*PGAIN*err;
  //      I = 2*IGAIN*err_sum;
  //      D = 2*DGAIN*err_diff;
  //    }
  //    else{
        P = PGAIN*err;
        I = IGAIN*err_sum;
        D = DGAIN*err_diff;
  //    }
      to_motor = int(to_motor + P + I + D);
      to_motor = constrain(to_motor,0,180);
      //Serial.println(to_motor);
      servo.write(to_motor);
    }
  }
}

//-----INTERUPT-HANDLERS-FOR-ULTRASONIC-SENSOR---


//volatile int state = LOW;
ISR(TIMER2_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
  if(trig != 0){ //if the echo pulse hasn't ended, give invalid trig value
    trig = -1;
  }
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(40);
  digitalWrite(trigPin, LOW);
}

void detect_echo(){
  if(trig != -1){
    int rising_edge = digitalRead(echoPin);
    if(rising_edge){
      trig = micros();
    }
    else if(trig != 0){
      diff = micros() - trig;
      distance = ((micros() - trig)/2.) / 29.1;
      trig = 0;
    }
  }
  else{
    distance = 999;
    trig = 0;
  }
}
