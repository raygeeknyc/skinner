import serial
import time

ser = serial.Serial('/dev/serial0', 250000, timeout=1)
data = "abcdefghijklmnopqrstuvwxyz"
data += "abcdefghijklmnopqrstuvwxyz"
data += "abcdefghijklmnopqrstuvwxyz"
data += "abcdefghijklmnopqrstuvwxyz"
ser.isOpen()
startsecs = time.time()
ret = ser.write(data) 
if ret != len(data):
	print "not written?"
databack=ser.read(len(data))
print "read back '%s'" % databack
for i in range(250):
	ret = ser.write(data) 
	if ret != len(data):
  		print "not written?"
	databack=ser.read(len(data))
	if len(databack) != ret:
  		print "only %d bytes read back" % len(databack)
print "It took %f seconds to round-trip %d bytes" % (time.time()-startsecs, 250*len(data))
ser.close()
