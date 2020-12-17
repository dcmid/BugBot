#include <Servo.h>

#define servoPin  10

const double PGAIN = 0.05;
const double IGAIN = 0;
const double DGAIN = 0.1;

Servo servo;

void setup(){
  Serial.begin(9600);
  servo.attach(servoPin);
  servo.write(90); //stationary
}

int set_pos = 254/2; //set point centers face in camera
int err;
int err_prev = 0;
int err_diff;
int err_sum = 0;
int P, I, D;
int to_motor = 90;
int face_pos;
void loop(){
  if(Serial.available()){
    face_pos = Serial.read();
    //Serial.println(face_pos);
    if(face_pos != 255){ //255 means no face was detected
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
      Serial.println("No Face Detcted");
    }
    if(abs(err) < 10): //if close enough, stop moving
      err_diff = 0;
      err_sum = 0;
      err_diff = 0;
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
    Serial.println(to_motor);
    servo.write(to_motor);
  }
}
