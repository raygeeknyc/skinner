import cv2
import serial
import time

print "Starting image processor"
frameTimerDuration = 1

# The desired resolution of the Pi camera
resolution = (320, 240)
# The desired maximum framerate, 0 for maximum possible throughput
framerate = 0
# The serial BAUD rate
BAUD = 230400
# The serial port to use
SERPORT = '/dev/serial0'

# These are the resolution of the output display, set these
displayWidth = 32.0
displayHeight = 16.0

# These are the horizontal margins of the input feed to crop, everything else scales to fit these
xLeft = 0
xRight = 0

# Open serial port at the highest tested standard BAUD rate
#ser = serial.Serial(SERPORT, BAUD, timeout=1)
#ser.isOpen()

# Open cam, decode image, show in window
cap = cv2.VideoCapture(0) # use 1 or 2 or ... for other camera
success, img = cap.read()
resolution = (len(img[0]), len(img))
print "input resolution is %d,%d" % resolution
print "target resolution is %d,%d" % (displayWidth, displayHeight)

cv2.namedWindow("Original")
cv2.namedWindow("Cropped")
cv2.namedWindow("Downsampled")

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

def writePixels(pixelData):
	print "row of %d bytes" % len(pixelData)
	return
	written = ser.write(pixel)
	if written != len(pixelData):
		print "error - wrote %d but only sent %d" % (len(pixelData), written)
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
		for row in small:
			print "sending row of %d pixels" % len(row)
			writePixels(row.tobytes())
		key = cv2.waitKey(1)
except KeyboardInterrupt as e:
	print "Interrupted"

stream.close()
cap.close()
camera.close()

cv2.destroyAllWindows()