from picamera import PiCamera
from picamera.array import PiRGBArray
import cv2
import time

print "Starting"
frameTimerDuration = 1

# This is the desired resolution of the Pi camera
resolution = (320, 240)
# This is the desired maximum framerate, 0 for maximum possible throughput
framerate = 0

# These are the resolution of the output display, set these
displayWidth = 32.0
displayHeight = 16.0

# These are the horizontal margins of the input feed, everything else scales to fit these
xLeft = 30
xRight = 30

camera = PiCamera()
camera.resolution = resolution
if framerate > 0:
	camera.framerate=framerate
cap = PiRGBArray(camera)
cap = PiRGBArray(camera, size=resolution)
stream = camera.capture_continuous(cap, format="rgb", use_video_port=True)

#cv2.namedWindow("Original")
#cv2.namedWindow("Cropped")
#cv2.namedWindow("Downsampled")

# read one frame to get its resolution
print("image resolution (x,y) %d,%d" % resolution)

_displayAspectRatio = displayHeight / displayWidth
_xMin = xLeft
_xMax = resolution[0]
_height = (_displayAspectRatio * resolution[1])
_yMin = (resolution[1] - _height)/2
_yMax = _yMin + _height

downsampleXFactor = displayWidth / _xMax
downsampleYFactor = displayHeight / _yMax
print "scaling (x,y) %f, %f" % (downsampleXFactor, downsampleYFactor)

frameTimer = time.time() + frameTimerDuration
frameCounter = 0
try:
	for frame in stream:
		img = frame.array
		cap.truncate(0)
		frameCounter += 1
		if time.time() > frameTimer:
			print "processed %d frames in %f seconds" % (frameCounter, frameTimerDuration)
			frameCounter = 0
			frameTimer = time.time() + frameTimerDuration
		cropImg = img[_yMin:_yMax,_xMin:_xMax] # this is all there is to cropping
		small = cv2.resize(img, (0,0), fx=downsampleXFactor, fy=downsampleYFactor) 
except KeyboardInterrupt as e:
	print "Interrupted"

#    cv2.imshow("Original", img)
#    cv2.imshow("Cropped", cropImg)
#    cv2.imshow("Downsampled", small)

stream.close()
cap.close()
camera.close()

cv2.destroyAllWindows()
