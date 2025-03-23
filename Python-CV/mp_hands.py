import config.args as arg
import config.osc as osc
import mediapipe as mp
import cv2
import math

#getting args
args = arg.get_args()

#drawing setup
mp_drawing = mp.solutions.drawing_utils

#model setup
mp_hands = mp.solutions.hands
hand = mp_hands.Hands()

#functions

def thumb_index_distance(thumb_landmark, index_landmark):

    thumb_point = [thumb_landmark.x, thumb_landmark.y]
    index_point = [index_landmark.x, index_landmark.y]

    distance = round(math.dist(thumb_point, index_point), 2)

    # Getting midpoint between index and thumb
    midpoint = [round((thumb_landmark.x + index_landmark.x)/2, 2), round((thumb_landmark.y + index_landmark.y)/2, 2)]
    #print(midpoint)


    return distance, midpoint

cap = cv2.VideoCapture(args.camDISP)

while cap.isOpened():
    # read frame
    _, frame = cap.read()

    if cv2.waitKey(1) == ord('q'):
        break

    cv2.namedWindow("Output", cv2.WINDOW_AUTOSIZE) 
    x, y, window_width, window_height = cv2.getWindowImageRect("Output")

    try:
        # convert to RGB
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        # process the frame for pose detection
        hands_results = hand.process(frame_rgb)
        
        # Check if hands_landmarks is not None
        if hands_results.multi_hand_landmarks is not None:

            for hand_landmarks in hands_results.multi_hand_landmarks:
                mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
                osc.send_landmarks_pd(hand_landmarks.landmark)

                t_i_distance, midpoint = thumb_index_distance(hand_landmarks.landmark[4],hand_landmarks.landmark[8])

                # Drawing line and point - pure display, we want the normalized values to pd, not
                cv2.line(frame, (int(hand_landmarks.landmark[4].x * window_width), int(hand_landmarks.landmark[4].y * window_height)), (int(hand_landmarks.landmark[8].x * window_width), int(hand_landmarks.landmark[8].y * window_height)), (255, 0, 0), 5)    
                cv2.circle(frame,(int(midpoint[0]*window_width),int(midpoint[1]*window_height)), 5, (0,0,255), -1)


        # display the frame
        cv2.imshow('Output', frame)

    except Exception as e:
        print(f"Error: {e}")
        break

cap.release()
cv2.destroyAllWindows()
