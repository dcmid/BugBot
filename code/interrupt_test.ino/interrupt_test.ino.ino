#define trigPin 13
#define echoPin 2

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT_PULLUP);
  cli(); //stop interrupts
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 4Hz increments
  OCR1A = 3656;// = (8*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
  attachInterrupt(digitalPinToInterrupt(echoPin), detect_echo, RISING);
}

volatile unsigned long distance = 0;
void loop() {
    delay(200);
    if (distance >= 200 || distance <= 0){
      Serial.println("Out of range");
    }
    else {
      Serial.print(distance);
      Serial.println(" cm");
    }
}

volatile unsigned long trig;
ISR(TIMER1_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   trig = micros();
//   long duration = pulseIn(echoPin, HIGH);
//   distance = (duration/2) / 29.1;
}

void detect_echo(){
  if(trig != 0){
    distance = ((micros() - trig)/2.) / 29.1;
    trig = 0;
  }
}
