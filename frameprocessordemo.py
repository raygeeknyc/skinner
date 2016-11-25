# Required modules
import cv2
import time

frameTimerDuration = 1
displayWidth = 32.0
displayHeight = 16.0
displayAspectRatio = displayHeight / displayWidth

# Open cam, decode image, show in window
cap = cv2.VideoCapture(0) # use 1 or 2 or ... for other camera
cv2.namedWindow("Original")
cv2.namedWindow("Cropped")
cv2.namedWindow("Downsampled")
key = -1
success, img = cap.read()
print("image resolution (x,y) %d,%d" % (len(img), len(img[0])))
xMin = 0
yMin = 0
xMax = 640
height = (displayAspectRatio * len(img))
yMin = (len(img) - height)/2
yMax = yMin + height

downsampleXFactor = displayWidth / xMax
downsampleYFactor = displayHeight / yMax
print "scaling (x,y) %f, %f" % (downsampleXFactor, downsampleYFactor)
frameTimer = time.time() + frameTimerDuration
print "frame sampling from %f to %f" % (time.time(), frameTimer)
frameCounter = 0
while(key < 0):
    success, img = cap.read()
    frameCounter += 1
    if time.time() > frameTimer:
        print "processed %d frames in %f seconds" % (frameCounter, frameTimerDuration)
        frameCounter = 0
        frameTimer = time.time() + frameTimerDuration
    cropImg = img[yMin:yMax,xMin:xMax] # this is all there is to cropping
    small = cv2.resize(cropImg, (0,0), fx=downsampleXFactor, fy=downsampleYFactor) 

    cv2.imshow("Original", img)
    cv2.imshow("Cropped", cropImg)
    cv2.imshow("Downsampled", small)

    key = cv2.waitKey(1)
cv2.destroyAllWindows()
