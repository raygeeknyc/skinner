from picamera import PiCamera
from picamera.array import PiRGBArray
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

# Open serial port at the highest tested standard BAUD rate
ser = serial.Serial(SERPORT, BAUD, timeout=1)
ser.isOpen()

# Open the video capture device
camera = PiCamera()
camera.resolution = resolution
if framerate > 0:
        camera.framerate=framerate
cap = PiRGBArray(camera)
stream = camera.capture_continuous(cap, format="rgb", use_video_port=True)

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
	print "row of %d pixels" % len(pixelData)
	rgbValues = bytearray(pixelData.tostring())
	written = ser.write(rgbValues)
	if written != len(rgbValues):
		print "error - wrote %d but only sent %d" % (len(rgbValues), written)

frameTimer = time.time() + frameTimerDuration
frameCounter = 0
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

		cv2.imshow("Original", img)
		cv2.imshow("Cropped", cropImg)
		cv2.imshow("Downsampled", small)
		for row in small:
			print "sending row of %d pixels" % len(row)
			writePixels(row)
		key = cv2.waitKey(1)
except KeyboardInterrupt as e:
	print "Interrupted"

stream.close()
ser.close()
cap.close()
camera.close()

cv2.destroyAllWindows()
