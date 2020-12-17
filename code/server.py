from flask import Flask, Response, render_template
#from imutils.video import VideoStream
import io
from flask_socketio import SocketIO, emit
import datetime
import imutils
import time
import cv2
#import paho.mqtt.client as mqtt
import serial
import numpy as np
import sys
from threading import Thread
import logging

# ser = serial.Serial('/dev/ttyUSB1', 9600)
# ser.flushInput()

# Arduino communication
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
        try:
            ser = serial.Serial('/dev/ttyUSB2', 9600)
            print("Arduino connected on USB2")
        except:
            print("Arduino not found on USB2")
            print("Arduino not found")
            sys.exit()
ser.flushInput()
def read_from_port(ser):
    while True:
        if(ser.in_waiting > 0):
            reading = ser.readline().decode().strip()
            #print(reading)
            socketio.emit('update_dist', reading)


app = Flask(__name__)
log = logging.getLogger('werkzeug')
log.disabled=True
socketio = SocketIO(app)

thread = Thread(target=read_from_port, args=[ser])
thread.start()

face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
IMG_PIXEL_WIDTH = 640
#used to scale x location to fit in byte
#254 left free to signify no face detected
#255 left as fire command
byte_scale = 253/IMG_PIXEL_WIDTH

print("Trying to open the Webcam.")
cap = cv2.VideoCapture(0)
if cap is None or not cap.isOpened():
    raise("No camera")

#---------Face-Detection------------
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

def gen():
    errs = np.zeros((10,1)) - 999; #set initial past centers to -999 (no face detected)
    while True:
        center, img = process_img() #get new image and face location
        if(center != -999):
            scaled_center = int(np.round(byte_scale*center))
            center_byte = bytes([scaled_center])
        else:
            center_byte = bytes([254])
        ser.write(center_byte)
        #print(avg_err)
        cv2.imwrite('pic.jpg', img)
        yield (b'--frame\r\n'b'Content-Type: image/jpeg\r\n\r\n' + open('pic.jpg', 'rb').read() + b'\r\n')

@app.route('/')
def index():
    return render_template("index.html")

@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen(),
                   mimetype='multipart/x-mixed-replace; boundary=frame')

@socketio.on('connect')
def test_connect():
    print('Client connected')
    emit('my response', {'data': 'Connected'})

@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected')

@socketio.on('send_it')
def send_it():
    ser.write(bytes([255])) #tell arduino to send it
    print('SUNT')


if __name__ == '__main__':
    app.run(debug=False, host='0.0.0.0', threaded=True)
    socketio.run(app)
