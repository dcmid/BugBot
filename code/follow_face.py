import numpy as np
import cv2
import sys
import serial

face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
IMG_PIXEL_WIDTH = 640
#used to scale x location to fit in byte
#255 left free to signify no face detected
byte_scale = 254/IMG_PIXEL_WIDTH

print("Connecting to the Arduino...")
try:
    ser = serial.Serial('/dev/ttyUSB0', 9600)
    print("Arduino connected on USB0")
except:
    print("Arduino not found on USB0")
    try:
        ser = serial.Serial('/dev/ttyUSB1', 9600)
        print("Arduino connected on USB1")
    except:
        print("Arduino not found on USB1")
        print("Arduino not found")
        sys.exit()
ser.flushInput()

print("Trying to open the Webcam.")
cap = cv2.VideoCapture(0)
if cap is None or not cap.isOpened():
    raise("No camera")

def process_img():
   ret, img = cap.read()

   gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

   ##centers = np.roll(centers,1) #shift previous centers right
   faces = face_cascade.detectMultiScale(gray, 1.3, 5)
   if(len(faces) > 0): #if a face is detected
       (x,y,w,h) = faces[0,:]#extract coordinates of first detected face
       center = (x+w/2) #centerpoint of face

       y0 = 0 #botton of image (for drawing vertical line)
       y1 = img.shape[0] - 1 #top of image
       img = cv2.line(img,(int(center),y0), (int(center),y1), (255,0,0), 2) #draw vertical line
   else:
       center = -999 #no face detected
   return center, img

while True:
    center, img = process_img() #get new image and face location
    if(center != -999):
        scaled_center = int(np.round(byte_scale*center))
        center_byte = bytes([scaled_center])
    else:
        center_byte = bytes([255])
    ser.write(center_byte)
    if cv2.waitKey(1) & 0xFF == ord('q'):
      cap.release()
      break
