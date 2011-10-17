#!/usr/local/bin/python
import serial
serialPort = serial.Serial("/dev/ttyACM0",baudrate=9600,timeout=0,rtscts=0,xonxoff=0)
serialPort.write("AT+CSCA\r")
