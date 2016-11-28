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

# These are the horizontal margins of the input feed to crop, everything else scales to fit these
xLeft = 0
xRight = 0

# Open cam, decode image, show in window
cap = cv2.VideoCapture(0) # use 1 or 2 or ... for other camera
success, img = cap.read()
resolution = (len(img[0]), len(img))
print "input resolution is %d,%d" % resolution

cv2.namedWindow("Original")
cv2.namedWindow("Cropped")
cv2.namedWindow("Downsampled")

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
	key = -1
	while(key < 0):
		success, img = cap.read()
		frameCounter += 1
		if time.time() > frameTimer:
			print "processed %d frames in %f seconds" % (frameCounter, frameTimerDuration)
			frameCounter = 0
			frameTimer = time.time() + frameTimerDuration
		cropImg = img[_yMin:_yMax,_xMin:_xMax]
		small = cv2.resize(cropImg, (0,0), fx=downsampleXFactor, fy=downsampleYFactor) 
		cv2.imshow("Original", img)
		cv2.imshow("Cropped", cropImg)
		cv2.imshow("Downsampled", small)
		print "Frame shape %d,%d" % (len(small[0]), len(small))
		print "%d bytes in frame data" % len(small.tostring())
		key = cv2.waitKey(1)
except KeyboardInterrupt as e:
	print "Interrupted"

stream.close()
cap.close()
camera.close()

cv2.destroyAllWindows()
