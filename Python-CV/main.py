import config.args as parser
import config.osc as osc
import mediapipe as mp
import cv2
import mp_hands 
import mp_pose

#getting args
args = parser.get_args()


cap = cv2.VideoCapture(args.cam)

while cap.isOpened():
    # read frame
    _, frame = cap.read()

    # mirrors frame
    frame = cv2.flip(frame, 1)

    # Press q to quit program
    if cv2.waitKey(1) == ord('q'):
        break

    # Getting window dimensions of output window - used to calculate some lines to draw, as MP returns normalized data  
    cv2.namedWindow("Output", cv2.WINDOW_AUTOSIZE) 
    x, y, window_width, window_height = cv2.getWindowImageRect("Output")

    try:

        # Executing chosen model
        match args.model:
            case "mp_hands":
                frame = mp_hands.hands_loop(frame, window_width, window_height)
          
            case "mp_pose":
                frame = mp_pose.pose_loop(frame)

            case "ZED":
                print("Model not implemented. Closing...")
                break

            case _:
                print("Model choice argument not accepted. Closing...")
                break
            

        # display the frame
        cv2.imshow('Output', frame)

    except Exception as e:
        print(f"Error: {e}")
        break

cap.release()

cv2.destroyAllWindows()


#TODO Multiple camera setup?