# Computervision data through Python to send to Pure Data

This is a collection of ML models that can track bodies, individual parts of bodies, or multiple people. This repo uses Mediapipe and ZED as model. The landmark data is then sent via OSC to Pure Data. The OSC data can be sent anywhere you might want. See folder with the Pure Data patches, for examples how to use this data.

The OSC communication was originally inspired by danielgomezmarion: https://github.com/danielgomezmarin/python_comm/ 

Their repo also contains information on how to get information back to Python. As this was out of scope for this project, it is not implemented in this current version.

## Model choices:
### mp_hands
Mediapipe's handtracking module. See references folder hand_landmarks.png for how the landmarks are split.

### mp_pose
Mediapipe's pose estimation module. See references folder pose_landmarks.png for how the landmarks are split.

### ZED
Not implemented as of yet.

## Setup
