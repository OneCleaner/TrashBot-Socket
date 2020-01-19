###################################
##### Authors:                #####
##### Stephane Vujasinovic    #####
##### Frederic Uhrweiller     #####
#####                         #####
##### Creation: 2017          #####
###################################

import numpy as np
import cv2
import sys

print('Starting the Calibration. Press and maintain the space bar to exit the script\n')
print('Push (s) to save the image you want and push (c) to see next frame without saving the image')

id_image=0

# termination criteria
criteria =(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# Call the two cameras
CamL= cv2.VideoCapture(0)   # 0 -> Right Camera

while True:
    retL, frameL= CamL.read()

    grayL= cv2.cvtColor(frameL,cv2.COLOR_BGR2GRAY)

    # Find the chess board corners
    retL, cornersL = cv2.findChessboardCorners(grayL,(6,10),None)  # Define the number of chess corners (here 9 by 6) we are looking for with the right Camera
    cv2.imshow('imgL',frameL)

    # If found, add object points, image points (after refining them)
    if (retL == True):
        corners2L= cv2.cornerSubPix(grayL,cornersL,(11,11),(-1,-1),criteria)    # Refining the Position

        # Draw and display the corners
        cv2.drawChessboardCorners(grayL,(6,10),corners2L,retL)
        cv2.imshow('VideoL',grayL)

        if cv2.waitKey(0) & 0xFF == ord('s'):   # Push "s" to save the images and "c" if you don't want to
            str_id_image= str(id_image)
            print('Images ' + str_id_image + ' saved for left camera')
            cv2.imwrite('chessboard-L'+str_id_image+'.png',frameL) # Save the image in the file where this Programm is located
            id_image=id_image+1
        else:
            print('Images not saved')

    # End the Programme
    if cv2.waitKey(1) & 0xFF == ord(' '):   # Push the space bar and maintan to exit this Programm
        break

# Release the Cameras
CamL.release()
cv2.destroyAllWindows()
sys.exit()
