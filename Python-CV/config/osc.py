from pythonosc import dispatcher 
from pythonosc import osc_server 
from pythonosc import udp_client
import config.args as arg
import numpy as np

args = arg.get_args()

client = udp_client.SimpleUDPClient(args.ipIN, args.portOUT)

## osc setup
def send_to_pd(path,data):
    client.send_message(path, data)

def send_landmarks_pd(landmarks):

    i = 0
    default_value = 0

    for landmark in landmarks:
        
        osc_message = []

        address = f"/landmark{i}"

        if landmark is None:
            print(f'No landmark detected for landmark {i}')

            # Adding default value to coordinates not found
            #TODO Find reasonable default value

            landmark_coords = (default_value, default_value, default_value)

            continue
        

        landmark_coords = (round(np.clip(landmark.x, 0, 1), 2)), (round(np.clip(landmark.y, 0, 1), 2)), (round(np.clip(landmark.z, 0, 1), 2))

        i += 1
        
        # Sending OSC message for landmark
        send_to_pd(address, landmark_coords)

    
    return 0