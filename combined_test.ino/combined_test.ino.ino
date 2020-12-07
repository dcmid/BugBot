#define trigPin 13
#define echoPin 2
#define servoPin  10

#include <Servo.h> 

Servo servo;

void setup() {
  Serial.begin(115200);
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
  TCCR2B |= (1 << CS22) | (1 << CS20);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();
  servo.attach(servoPin);
//  servo.write(90); //stationary
  //attachInterrupt(digitalPinToInterrupt(echoPin), detect_echo, RISING);
}

#define DESIRED_DIST 20 //cm
volatile unsigned long distance = 0;
void loop() {
    delay(200);
    int err;
    int to_serv;
    if (distance >= 200 || distance <= 0){
//      Serial.println("Out of range");
      to_serv = 180; //full steam ahead!
    }
    else {
//      Serial.print(distance);
//      Serial.println(" cm");
      err = distance - DESIRED_DIST;
      to_serv = constrain(90+err,0,180);
    }
    Serial.println(to_serv);
    //servo.write(to_serv);
}

volatile unsigned long trig;
volatile unsigned long cnt;
ISR(TIMER2_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
  cnt++;
  if(cnt > 10){
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    //   trig = micros();
    long duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    cnt = 0;
  }
}

void detect_echo(){
  if(trig != 0){
    long distance = ((micros() - trig)/2.) / 29.1;
    trig = 0;
    if (distance >= 200 || distance <= 0){
      Serial.println("Out of range");
    }
    else {
      Serial.print(distance);
      Serial.println(" cm");
    }
  }
}
