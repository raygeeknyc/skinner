import numpy
import cv2
import time

print "Starting demo"
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
print "target resolution is %d,%d" % (displayWidth, displayHeight)

cv2.namedWindow("Original")
cv2.namedWindow("Cropped")
cv2.namedWindow("Downsampled")
cv2.namedWindow("Equalized")
cv2.namedWindow("Contrast")

_displayAspectRatio = displayHeight / displayWidth
print "aspect ratio %f" % _displayAspectRatio
_xMin = xLeft
_xMax = resolution[0]-xRight
_height = int(_displayAspectRatio * resolution[0])
_width = _xMax+1 - _xMin
_yMin = int((resolution[1] - _height)/2)
_yMax = _yMin + _height
print "min = %d, max = %d, height = %d" % (_yMin, _yMax, _height)

downsampleXFactor = displayWidth / _width
downsampleYFactor = displayHeight / _height
print "Crop to (%d,%d)=%d:(%d,%d)=%d" % (_xMin,_xMax,(_xMax-_xMin),_yMin,_yMax,(_yMax-_yMin))
print "Scaling by (x,y) %f, %f" % (downsampleXFactor, downsampleYFactor)
print "Scales to (%d,%d)" % (_width*downsampleXFactor,_height*downsampleYFactor)

def equalize_brightness(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    hsv[:,:,2] = cv2.equalizeHist(hsv[:,:,2])
    img = cv2.cvtColor(hsv, cv2.COLOR_HSV2BGR) 
    return img

def equalize_hist(img):
    for c in xrange(0, 2):
       img[:,:,c] = cv2.equalizeHist(img[:,:,c])
    return img

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
		smallImg = cv2.resize(cropImg, (0,0), fx=downsampleXFactor, fy=downsampleYFactor) 
		equalizedImg = numpy.copy(cropImg)
		contrastImg = numpy.copy(cropImg)
		equalize_hist(equalizedImg)
		contrastImg = equalize_brightness(contrastImg)
		cv2.imshow("Original", img)
		cv2.imshow("Cropped", cropImg)
		cv2.imshow("Downsampled", smallImg)
		cv2.imshow("Equalized", equalizedImg)
		cv2.imshow("Contrast", contrastImg)
		key = cv2.waitKey(1)
except KeyboardInterrupt as e:
	print "Interrupted"

stream.close()
cap.close()
camera.close()

cv2.destroyAllWindows()
