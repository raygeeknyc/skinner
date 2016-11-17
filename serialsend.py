import serial
ser = serial.Serial('/dev/ttyS0', 115200, timeout=1)
data = "abcdefghijklmnopqrstuvwxyz"
ser.isOpen()
ret = ser.write(data) 
if ret != len(data):
  print "not written?"
databack=ser.read(len(data))
print "read back '%s' (%d)" % (data, len(data))
ser.close()
