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
### General setup
You need Python version between 3.10-3.12, as mediapipe only supports these versions as of now, and due to some code in the program. 
You need to install the dependencies from requirements.txt in either a virtual environment (.venv OR .conda), or to your root Python.

#### Requirements
- Text editor (E.g. Visual Studio Code, Sublime, Atom, etc.)
- Camera (Regular webcam works fine for Mediapipe models, but ZED camera is required for ZED models.)

#### Tutorials to pip install:
NOTE: For all tutorials, once you learn how to pip install a library, just replace whatever library with "requirements.txt". This will install required libraries.

(Videos)

(Recommended) Setting up a virtual environment (.venv) in Python in Virtual Studio Code: 
https://www.youtube.com/watch?v=GZbeL5AcTgw

Installing Python libraries to the root: 
https://www.youtube.com/watch?v=7snh_1Hf_TI

(Text guide)

Getting started with Python in Visual Studio Code
https://code.visualstudio.com/docs/python/python-tutorial

### How to use
With everything set up and installed, we are now ready to run!
To use the program, run the main.py script, either directly from your editor (Visual Studio Code has a 'play' button as an example), or run it from your terminal.
If you run it directly it should look something like this:

![Hands being tracked with Mediapipe](images/mp_hands.png)

By default, the program runs with the mp_hands model as displayed in the image above.

If you have the accompanying Pure Data patch open, you should see the values begin to change as you move your hands about:

![Hands being tracked with Mediapipe, altering Pure Data values](images/mp_hands_pd.gif)

#### How to change settings
You might want to change some settings, depending on your setup. These settings can be changed with arguments when running the script from the terminal, or be changed directly in the script if you want a new default. 
##### Setting arguments in terminal when running the script
To set the arguments when running the script, follow up the script name with the argument you wish to change, followed by the new argument. You can change any argument that allows it when running the script. E.g. "py main.py --model mp_pose --cam 2". This would change the model of the script, and the camera which is used.

Possible arguments:
- --ipIN: The IP address of the devices you want to connect
- --portOUT: The port which the data is sent
- --cam: The camera you want to use, expressed as an integer. 0 defaults to integrated camera if present, or first registered .
- --model: AI tracking model. Possible arguments:
  - mp_hands
  - mp_pose

##### Changing default settings
Go the the 'config' folder and choose 'args.py'. Find the settings you want change, and change.

## Inspirations:
- pepepepebrick on Instagram: https://www.instagram.com/p/DCwbZwczaER/
- danielgomezmarion: https://github.com/danielgomezmarin/

#TODO Insert images and gifs