import cv2
import mediapipe as mp
import numpy as np
import config.args as arg
import config.osc as osc

## initialize pose estimator
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose
pose = mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5)
##

args = arg.get_args()

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
            osc.send_landmarks_pd(pose_results.pose_landmarks.landmark)

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