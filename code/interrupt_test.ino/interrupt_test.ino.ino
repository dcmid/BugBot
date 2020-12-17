#define trigPin 13
#define echoPin 2

void setup() {
  Serial.begin(115200);
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
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();
  attachInterrupt(digitalPinToInterrupt(echoPin), detect_echo, CHANGE);
}

volatile unsigned long diff = 0;
volatile unsigned long distance = 0;
volatile unsigned long trig = 0;
void loop() {
    delay(200);
    if (distance >= 200 || distance <= 0){
      Serial.println("Out of range");
    }
    else {
      Serial.print(distance);
      Serial.println(" cm");
    }
  //Serial.println(diff);
}

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
