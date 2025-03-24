import mediapipe as mp
import config.args as parser
import config.osc as osc

## initialize pose estimator
mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose
pose = mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5)
##

args = parser.get_args()

def pose_loop(frame):
            
    # process the frame for pose detection
    pose_results = pose.process(frame)
    #print(pose_results.pose_landmarks)
    
    # Check if pose_landmarks is not None
    if pose_results.pose_landmarks is not None:

        # Send landmarks to pd
        osc.send_landmarks_pd(pose_results.pose_landmarks.landmark)

        # Draw skeleton on the frame
        mp_drawing.draw_landmarks(frame, pose_results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
    
    else:
        print("No landmarks detected")
    
    return frame

