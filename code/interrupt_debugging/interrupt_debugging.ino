#define trigPin 13
#define echoPin 2

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(echoPin, LOW); //disable pullup
  cli(); //stop interrupts
  TCCR1A = 0;// set entire TCCR2A register to 0
  TCCR1B = 0;// same for TCCR2B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register
  OCR1A = 150000;// = set max comparison value
  // turn on CTC mode
  TCCR1A |= (1 << WGM11);
  // Set bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS20);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();
  attachInterrupt(digitalPinToInterrupt(echoPin), btn_pressed, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:

}

volatile unsigned long trig;
ISR(TIMER1_COMPA_vect){  //change the 0 to 1 for timer1 and 2 for timer2
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   trig = micros();
//   //long duration = pulseIn(echoPin, HIGH);
//   //distance = (duration/2) / 29.1;
}

void btn_pressed(){
  unsigned long diff = micros() - trig;
  Serial.println(diff);
}
