from picamera import PiCamera
from picamera.array import PiRGBArray
import cv2
import numpy
import serial
import sys
import time

_DEBUG = False
#_DEBUG = True

equalize = False
if len(sys.argv) > 1 and sys.argv[1] == "equalize":
	equalize = True

if _DEBUG:
	print "Starting image processor"
	if equalize:
		print "Equalizing brightness"
frameTimerDuration = 1

# How many frames to capture before recalculating average brightness
brightnessFrameSampleDuration = 100

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

FOOTER_LENGTH = 330

# These are the horizontal margins of the input feed to crop, everything else scales to fit these
xLeft = 90
xRight = 90

# These are the bytes of our frame header, used to sync frame boundaries on the receiver
FRAME_HEADER_1 = chr(int('0x01',16))
FRAME_HEADER_2 = chr(int('0x02',16))
FRAME_HEADER_3 = chr(int('0x03',16))
FRAME_FOOTER = chr(int('0x00',16))
# This is the number of bytes to send as the frame brightness
HEADER_BRIGHTNESS_LENGTH = 5

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

def equalize_hist(img):
    equalized = numpy.copy(img)
    for c in xrange(0, 2):
       equalized[:,:,c] = cv2.equalizeHist(equalized[:,:,c])
    return equalized

def get_brightness(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
    averageBrightness = int(cv2.mean(hsv[:,:,2])[0])
    return bytearray(format(averageBrightness,"0" + str(HEADER_BRIGHTNESS_LENGTH) + "d"))

def equalize_brightness(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
    hsv[:,:,2] = cv2.equalizeHist(hsv[:,:,2])
    rgb = cv2.cvtColor(hsv, cv2.COLOR_HSV2RGB)
    return rgb

if _DEBUG:
	cv2.namedWindow("Original")
	cv2.namedWindow("Cropped")
	cv2.namedWindow("Downsampled")

_displayAspectRatio = displayHeight / displayWidth
print "aspect ratio %f" % _displayAspectRatio
_xMin = xLeft
_xMax = resolution[0]-xRight
_width = _xMax+1 - _xMin
_height = int(_displayAspectRatio * _width)
_yMin = int((resolution[1] - _height)/2)
_yMax = _yMin + _height
print "min = %d, max = %d, height = %d" % (_yMin, _yMax, _height)

downsampleXFactor = displayWidth / _width
downsampleYFactor = displayHeight / _height
print "Crop to (%d,%d)=%d:(%d,%d)=%d" % (_xMin,_xMax,(_xMax-_xMin),_yMin,_yMax,(_yMax-_yMin))
print "Scaling by (x,y) %f, %f" % (downsampleXFactor, downsampleYFactor)
print "Scales to (%d,%d)" % (_width*downsampleXFactor,_height*downsampleYFactor)

def writeFrameHeader(brightness):
	written = ser.write(FRAME_HEADER_1)
	if written != 1:
		print "failed to send header 1"
		return False
	written = ser.write(FRAME_HEADER_2)
	if written != 1:
		print "failed to send header 2"
		return False
	written = ser.write(brightness)
	if written != HEADER_BRIGHTNESS_LENGTH:
		print "failed to send frame brightness"
		return False
	written = ser.write(FRAME_HEADER_3)
	if written != 1:
		print "failed to send header 3"
		return False
	return True

def writeFrameFooter():
	for i in range(FOOTER_LENGTH):
		written = ser.write(FRAME_FOOTER)
		if written != 1:
			print "failed to send footer"
			return False
	return True

def writeFrame(image):
	for row in image:
		writePixels(row)

def writePixels(pixelData):
	if _DEBUG:
		for pixel in pixelData:
			rgbPixel = bytearray(pixel.tostring())
			print "pixel(%d): (%d,%d,%d)" % (len(rgbPixel), rgbPixel[0],rgbPixel[1],rgbPixel[2])
	rgbValues = bytearray(pixelData.tostring())
	written = ser.write(rgbValues)
	if written != len(rgbValues):
		print "error - wrote %d but only sent %d" % (len(rgbValues), written)

frameTimer = time.time() + frameTimerDuration
frameCounter = 0
frameTimer = time.time() + frameTimerDuration
frameCounter = 0
try:
	print "reading stream"
        for frame in stream:
                img = frame.array
                cap.truncate(0)
                if frameCounter > brightnessFrameSampleDuration or not frameCounter:
			brightness = get_brightness(img)
                frameCounter += 1
                if _DEBUG and time.time() > frameTimer:
                        print "processed %d frames in %f seconds" % (frameCounter, frameTimerDuration)
                        frameCounter = 0
                        frameTimer = time.time() + frameTimerDuration
                cropImg = img[_yMin:_yMax,_xMin:_xMax]
		if equalize:
			cropImg = equalize_brightness(cropImg)
                small = cv2.resize(cropImg, (0,0), fx=downsampleXFactor, fy=downsampleYFactor)
		if _DEBUG:
			print "derez to {y=%d,x=%d}" % (len(small),len(small[0]))
		writeFrameHeader(brightness);
		writeFrame(small);
		writeFrameFooter();

		if _DEBUG:
			cv2.imshow("Original", img)
			cv2.imshow("Cropped", cropImg)
			cv2.imshow("Downsampled", small)
			key = cv2.waitKey(1)

except KeyboardInterrupt as e:
	print "Interrupted"

stream.close()
ser.close()
cap.close()
camera.close()

if _DEBUG:
	cv2.destroyAllWindows()
