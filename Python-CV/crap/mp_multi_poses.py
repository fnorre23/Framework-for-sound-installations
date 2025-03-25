import mediapipe as mp
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
from mediapipe import solutions
from mediapipe.framework.formats import landmark_pb2
import cv2
import config.args as parser
import numpy as np

args = parser.get_args()

mp_drawing = mp.solutions.drawing_utils
mp_pose = mp.solutions.pose

model_path = "C:\\Users\\frede\\source\\repos\\MED4\\Python-OSC-CV\\Python-CV\\models\\pose_landmarker_lite.task"

BaseOptions = mp.tasks.BaseOptions
PoseLandmarker = mp.tasks.vision.PoseLandmarker
PoseLandmarkerOptions = mp.tasks.vision.PoseLandmarkerOptions
VisionRunningMode = mp.tasks.vision.RunningMode

options = PoseLandmarkerOptions(
    base_options=BaseOptions(model_asset_path=model_path),
    running_mode=VisionRunningMode.IMAGE,
    num_poses=2)

def draw_landmarks_on_image(rgb_image, detection_result):
  pose_landmarks_list = detection_result.pose_landmarks
  annotated_image = np.copy(rgb_image)

  # Loop through the detected poses to visualize.
  for idx in range(len(pose_landmarks_list)):
    pose_landmarks = pose_landmarks_list[idx]

    # Draw the pose landmarks.
    pose_landmarks_proto = landmark_pb2.NormalizedLandmarkList()
    pose_landmarks_proto.landmark.extend([
      landmark_pb2.NormalizedLandmark(x=landmark.x, y=landmark.y, z=landmark.z) for landmark in pose_landmarks
    ])
    solutions.drawing_utils.draw_landmarks(
      annotated_image,
      pose_landmarks_proto,
      solutions.pose.POSE_CONNECTIONS,
      solutions.drawing_styles.get_default_pose_landmarks_style())
  return annotated_image

with PoseLandmarker.create_from_options(options) as landmarker:
  # The landmarker is initialized. Use it here.

    cap = cv2.VideoCapture(args.cam)

    while True:
        # read frame
        _, frame = cap.read()

        # mirrors frame
        frame = cv2.flip(frame, 1)

        # Press q to quit program
        if cv2.waitKey(1) == ord('q'):
            break

        # Convert the frame received from OpenCV to a MediaPipe’s Image object.
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=frame)

        # The pose landmarker must be created with the image mode.
        pose_landmarker_result = landmarker.detect(mp_image)    

        print(pose_landmarker_result.pose_landmarks)
        annotated_image = draw_landmarks_on_image(mp_image.numpy_view(), pose_landmarker_result)
        
       # cv2.imshow(cv2.cvtColor(annotated_image, cv2.COLOR_RGB2BGR))     
        cv2.imshow('Output', annotated_image)

    
    cap.release()
    cv2.destroyAllWindows()
