import config.args as parser
import config.osc as osc
import mediapipe as mp
import cv2
import math

#getting args
args = parser.get_args()

#drawing setup
mp_drawing = mp.solutions.drawing_utils

#model setup
mp_hands = mp.solutions.hands
hand = mp_hands.Hands()

#functions

def thumb_index_relation(thumb_landmark, index_landmark):

    thumb_point = [thumb_landmark.x, thumb_landmark.y]
    index_point = [index_landmark.x, index_landmark.y]

    distance = round(math.dist(thumb_point, index_point), 2)

    # Getting midpoint between index and thumb
    midpoint = [round((thumb_landmark.x + index_landmark.x)/2, 2), round((thumb_landmark.y + index_landmark.y)/2, 2)]

    return distance, midpoint
 
# loop

def hands_loop(frame, window_width, window_height):
        
        # process the frame for pose detection
        hands_results = hand.process(frame)
        
        # guard clause - checking if hand landmarks are present
        if hands_results.multi_hand_landmarks is None:

            return frame

        # TODO Tænke over om mere end 1 par hænder skal kunne bruges, i så fald skal der også tages højde for index og ikke bare handedness
        midpoints = []

        for idx, hand_landmarks in enumerate(hands_results.multi_hand_landmarks):

            # drawing hand
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # Getting left or right hand
            hand_label = hands_results.multi_handedness[idx].classification[0].label

            # Relationship between thumb and index the distance in 2D #TODO Skal det være 3d?
            # and the midpoint between the 2.
            t_i_distance, midpoint = thumb_index_relation(hand_landmarks.landmark[4],hand_landmarks.landmark[8])

            # adding midpoint to list
            midpoints.append(midpoint)

            # Sending landmarks to pd PD
            osc.send_landmarks_pd(hand_landmarks.landmark, hand_label)

            # Sending distance between thumb and index
            osc.send_to_pd(f'/{hand_label}/relation', t_i_distance)
    
            # Drawing line and point - pure display, we want the normalized values to pd
            cv2.line(frame, (int(hand_landmarks.landmark[4].x * window_width), int(hand_landmarks.landmark[4].y * window_height)), (int(hand_landmarks.landmark[8].x * window_width), int(hand_landmarks.landmark[8].y * window_height)), (255, 0, 0), 5)    
            cv2.circle(frame,(int(midpoint[0]*window_width),int(midpoint[1]*window_height)), 5, (0,0,255), -1)

        # Getting midpoints distance if 2 or more are present
        if len(midpoints) >= 2:

            # calculating distance between midpoints
            midpoint_dist = round(math.dist(midpoints[0], midpoints[1]), 2)

            # sending to pd
            osc.send_to_pd("/midpoint-distance", midpoint_dist)
            
            # Drawing lines between midpoints
            cv2.line(frame, (int(midpoints[0][0]*window_width), int(midpoints[0][1]*window_height)), (int(midpoints[1][0]*window_width), int(midpoints[1][1]*window_height)), (0,0,255), 5)
        
        return frame
