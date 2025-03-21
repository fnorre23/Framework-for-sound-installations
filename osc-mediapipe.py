import cv2
import mediapipe as mp
import numpy as np

import argparse
from pythonosc import dispatcher 
from pythonosc import osc_server 
from pythonosc import udp_client

import random
import time
import threading  # Add threading for parallel execution

#TODO Lav OSC setup til et modul, og så laver scripts til hver relevant computervision ting i guess - så kan man nemmere navigere de forskellige ting i 1 pakkeløsning

## initialize pose estimator
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose
pose = mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5)
##

## arguments for OSC
# set input and output port numbers
input_port = 5005
output_port = 5006

# Camera to work with
cam = 0

## osc setup
# Det er sådan her vi sender shit til PD
def send_to_pd(address):
    for i in range(1, 10):
        # Simulating sensor data by generating random values for each sensor
        sensor1 = random.random()  # Sensor 1 value
        sensor2 = random.random()  # Sensor 2 value
        sensor3 = random.random()  # Sensor 3 value
        
        # Vi skal filtrere data der matcher sidst sendt data, for at mindske pakkerne der sendes, så unødvendig data ikke sendes
        sensordata = ("sensor1", sensor1, "sensor2", sensor2, "sensor3", sensor3)

        # Send the sensor values as named messages
        talk2pd(args.ipIN, args.portOUT, address, sensordata)

        time.sleep(1)
        print(sensordata)  

def talk2pd(ip,port,path,mymove):
    client = udp_client.SimpleUDPClient(ip,port)
    client.send_message(path, mymove)

def main(path: str, *osc_arguments):
    msg = osc_arguments[-1]

def listen2pd(addrIN,addrOUT):
    ipIN   = addrIN[0]
    portIN = addrIN[1]
    pathIN = addrIN[2]
    disp = dispatcher.Dispatcher()
    disp.map(pathIN, main, ipIN)
    
    ###############################################################
    ############ call functions that use data from pd ############
    ###############################################################

    # handlers are functions we want to run with pd information
    #disp.map(address = "/myfunction1", handler = myfunction1) 
    #disp.map(address = "/myfunction2", handler = myfunction2) 
    ###############################################################

    # server to listen
    server = osc_server.ThreadingOSCUDPServer((ipIN,portIN), disp)
    #print("listening on {}".format(server.server_address))
    server.serve_forever()


# Sending each landmark coords as separate messages - data is routed in PD
def send_landmarks_pd(landmarks):

    i = 0

    for landmark in landmarks.landmark:
        
        osc_message = []

        address = f"/landmark{i}"

        default_value = 0

        if landmark is None:
            print(f'No landmark detected for landmark {i}')

            # Adding default value to coordinates not found
            #TODO Find reasonable default value

            landmark_coords = (default_value, default_value, default_value)

            continue
        

        landmark_coords = (round(np.clip(landmark.x, 0, 1), 2)), (round(np.clip(landmark.y, 0, 1), 2)), (round(np.clip(landmark.z, 0, 1), 2))

        i += 1
        
        # Sending OSC message for landmark
        talk2pd(args.ipIN, args.portOUT, address, landmark_coords)

    
    return 0

# eksempel output for en index i .pose_landmarks listen:
#x: 0.35414522886276245
#y: 0.8367241024971008
#z: 0.16406674683094025
#visibility: 0.9696751236915588



## parser
# generate parser
parser = argparse.ArgumentParser(prog='python_comm', formatter_class=argparse.RawDescriptionHelpFormatter, description='Receive and send OSC messages from pd')
parser.add_argument("-II","--ipIN", type=str, default="127.0.0.1", help="The ip to listen on")
parser.add_argument("-PI", "--portIN", type=int, default=input_port, help="The port to listen on")
parser.add_argument("-UI", "--uripathIN", type=str, default="/filter", help="PD's URI path")
parser.add_argument("-PO", "--portOUT", type=int, default=output_port, help="The port to send messages to")
parser.add_argument("-UO", "--uripathOUT", type=str, default="/filter", help="output URI path")
parser.add_argument("-CO", "--camDISP"), type=int, default=cam, help="The camera you use for the script"
args = parser.parse_args()
# wrap up inputs
outputAddress = [args.portOUT, args.uripathOUT]
inputAddress = [args.ipIN, args.portIN, args.uripathIN]

# Nedenstående skal nok i while loop ift ZED data, men også bare at det skal køre for evigt, for lige nu stopper det efter function4 er færdig
# Run the OSC server in a separate thread
server_thread = threading.Thread(target=listen2pd, args=(inputAddress, outputAddress))
server_thread.daemon = True  # Ensures the thread closes when the script stops
server_thread.start()

############
# Main
###########

cap = cv2.VideoCapture(args.camDISP)

while cap.isOpened():
    # read frame
    _, frame = cap.read()

    if cv2.waitKey(1) == ord('q'):
        break

    try:
        # convert to RGB
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        # process the frame for pose detection
        pose_results = pose.process(frame_rgb)
        #print(pose_results.pose_landmarks)
        
        # Check if pose_landmarks is not None
        if pose_results.pose_landmarks is not None:

            # Send landmarks to pd
            send_landmarks_pd(pose_results.pose_landmarks)

            # Draw skeleton on the frame
            mp_drawing.draw_landmarks(frame, pose_results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
        
        else:
            print("No landmarks detected")

        # display the frame
        cv2.imshow('Output', frame)

    except Exception as e:
        print(f"Error: {e}")
        break

cap.release()
cv2.destroyAllWindows()