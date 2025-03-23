import argparse

# generate parser
def get_args():
        
    ## arguments for OSC
    # set input and output port numbers
    input_port = 5005
    output_port = 5006

    # Camera to work with
    cam = 0
    
    # network
    parser = argparse.ArgumentParser(prog='python_comm', formatter_class=argparse.RawDescriptionHelpFormatter, description='Send OSC messages to pd')
    parser.add_argument("-II","--ipIN", type=str, default="127.0.0.1", help="The ip to listen on")
    parser.add_argument("-PI", "--portIN", type=int, default=input_port, help="The port to listen on")
    parser.add_argument("-UI", "--uripathIN", type=str, default="/filter", help="PD's URI path")
    parser.add_argument("-PO", "--portOUT", type=int, default=output_port, help="The port to send messages to")
    parser.add_argument("-UO", "--uripathOUT", type=str, default="/filter", help="output URI path")

    # camera
    parser.add_argument("-CO", "--camDISP", type=int, default=cam, help="The camera you use")

    # system choice
    #TODO Add system choice (ZED, Mediapipe, etc.)
    #TODO Add model choice for each system (Mediapie pose, mediapipe hands, etc.)


    args = parser.parse_args()
    
    return args