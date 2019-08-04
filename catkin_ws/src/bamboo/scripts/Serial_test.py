#!/usr/bin/env python         
# -*- coding: utf-8 -*-

import time
import serial
import rospy



def main():
    portname = "/dev/ttyUSB0"
    baudrate = 115200

    try:
        serialPort = serial.Serial(portname, baudrate)
        serialPort.writeTimeout = 2 #写超时
        print ("串口打开成功%s"%portname)
        print (",波特率为%s"%baudrate)

        
    except Exception as e:
        print ("机械臂串口被占用，请重新启动")
        print(e)
        exit(1)
    
    while True:
        serialPort.write("csascsc")
        time.sleep(1)
if __name__ == '__main__':
    main()
