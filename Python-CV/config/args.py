import argparse

# generate parser
def get_args():
    
    # IP address
    ip_address = "127.0.0.1"

    # set input and output port numbers - change here for default
    output_port = 5006

    # Camera to work with - change here for default
    cam = 0

    # Model choice - change here for default
    mod_choice = "mp_hands"

    # Generating parser
    parser = argparse.ArgumentParser(prog='python_comm', formatter_class=argparse.RawDescriptionHelpFormatter, description='Send OSC messages to pd')
    
    # network arguments
    parser.add_argument("-II","--ipIN", type=str, default=ip_address, help="The ip to listen on")
    parser.add_argument("-PO", "--portOUT", type=int, default=output_port, help="The port to send messages to")

    # camera arguments
    parser.add_argument("-CAM", "--cam", type=int, default=cam, help="The camera you use")

    # model choice arguments
    #TODO Add model choice for each system (Mediapie pose, mediapipe hands, etc.)
    parser.add_argument("-MOD", "--model", type=str, default=mod_choice, help="The model for tracking. See README or main.py for options")

    args = parser.parse_args()
    
    return args