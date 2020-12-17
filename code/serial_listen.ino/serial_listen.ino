void setup(){
  Serial.begin(9600);
}

//int set_pos = 254/2; //set point centers face in camera
byte message;
void loop(){
  if(Serial.available()){
    face_pos = Serial.read();
    Serial.println(message);
  }
}
