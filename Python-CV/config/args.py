import argparse

# generate parser
def get_args():
        
    # set input and output port numbers - change here for default
    input_port = 5005
    output_port = 5006

    # Camera to work with - change here for default
    cam = 0

    # Model choice - change here for default
    mod_choice = "mp_hands"

    # network
    parser = argparse.ArgumentParser(prog='python_comm', formatter_class=argparse.RawDescriptionHelpFormatter, description='Send OSC messages to pd')
    parser.add_argument("-II","--ipIN", type=str, default="127.0.0.1", help="The ip to listen on")
    parser.add_argument("-PI", "--portIN", type=int, default=input_port, help="The port to listen on")
    parser.add_argument("-PO", "--portOUT", type=int, default=output_port, help="The port to send messages to")

    # camera
    parser.add_argument("-CO", "--camDISP", type=int, default=cam, help="The camera you use")

    # system choice
    #TODO Add system choice (ZED, Mediapipe, etc.)
    #TODO Add model choice for each system (Mediapie pose, mediapipe hands, etc.)
    parser.add_argument("-MOD", "--modCHOICE", type=str, default=mod_choice, help="The model for tracking. See README or main.py for options")

    args = parser.parse_args()
    
    return args