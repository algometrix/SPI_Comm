#!/usr/bin/python
import time
import spidev
def setup(bus,channel):
	s=spidev.SpiDev()
	s.open(bus,channel)
	return s

def write_register(stream,address,val):
	data=[0x2]+[address,val]
	stream.xfer(data,500000)

def read_register(stream,address):
	data=[0x1]+[address]
	stream.xfer(data,500000)[0]
	return stream.xfer(data,500000)[0]

def read_bytes(stream):
	return stream.xfer2([0x4,1,2,3],500000)


if __name__ == '__main__':
	s=setup(0,1)
	
	#write_register(s,0x1,10)
	#write_register(s,0x2,3)
	#write_register(s,0x3,2)
	#print('1 : '+str(read_register(s,0x1)))
	#print('2 : '+str(read_register(s,0x2)))
	#print('3 : '+str(read_register(s,0x3)))
	try:
  		#Some stuff might raise an IO exception
		while(True):
			print(read_bytes(s))
			time.sleep(1)	
	except KeyboardInterupt :
		s.close()
