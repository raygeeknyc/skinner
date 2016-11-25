# Required modules
import cv2
import time

frameTimerDuration = 1

# these are the resolution of the output display, set these
displayWidth = 32.0
displayHeight = 16.0
# These are the horizontal margins of the input feed, everything else scales to fit these
xLeft = 30
xRight = 30


# Open cam, decode image, show in window
cap = cv2.VideoCapture(0) # use 1 or 2 or ... for other camera
cv2.namedWindow("Original")
cv2.namedWindow("Cropped")
cv2.namedWindow("Downsampled")

# read one frame to get its resolution
success, img = cap.read()
print("image resolution (x,y) %d,%d" % (len(img), len(img[0])))

_displayAspectRatio = displayHeight / displayWidth
_xMin = xLeft
_xMax = len(img[0])-xRight
_height = (_displayAspectRatio * len(img))
_yMin = (len(img) - _height)/2
_yMax = _yMin + _height

downsampleXFactor = displayWidth / _xMax
downsampleYFactor = displayHeight / _yMax
print "scaling (x,y) %f, %f" % (downsampleXFactor, downsampleYFactor)

frameTimer = time.time() + frameTimerDuration
frameCounter = 0
key = -1
while(key < 0):
    success, img = cap.read()
    frameCounter += 1
    if time.time() > frameTimer:
        print "processed %d frames in %f seconds" % (frameCounter, frameTimerDuration)
        frameCounter = 0
        frameTimer = time.time() + frameTimerDuration
    cropImg = img[_yMin:_yMax,_xMin:_xMax] # this is all there is to cropping
    small = cv2.resize(cropImg, (0,0), fx=downsampleXFactor, fy=downsampleYFactor) 

    cv2.imshow("Original", img)
    cv2.imshow("Cropped", cropImg)
    cv2.imshow("Downsampled", small)

    key = cv2.waitKey(1)
cv2.destroyAllWindows()
