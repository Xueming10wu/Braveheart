#!/usr/bin/env python         
# -*- coding: utf-8 -*-

import time
import serial
import rospy




    #读取某个舵机的脉宽(角度)，返回0则是读取数据失败
def read(joint):
        command = ""
        command += "#"
        command += str(joint)
        command += "PRAD\r\n"
        
        try:
            flage_ = serialPort.write(command)
        except Exception as e:
            return 0
        
        flage_ = 5

        if flage_ > 0:
            #真正连接舵机时使用
            respond = serialPort.readLine()

            respond = raw_input("输入舵机返回一个数据 >> ")
            respondList = list(respond)
            joint_ = int(respond[1]) * 100 + int(respond[2]) * 10 + int(respond[3])
            puls_width_ =int(respond[5]) * 1000 + int(respond[6]) * 100 + int(respond[7]) * 10 + int(respond[8])
            
            print(joint_)
            print(puls_width_)

            if joint_ == joint :
                return puls_width_
            else:
                return 0
        else:
            return 0
        
        
    #写某个舵机的脉宽(角度), 返回是否成功写入
def write(joint, puls_width, duration ):
        command = ""
        command += "#"
        command += str(joint)
        command += "P"
        command += str(puls_width)
        command += "T"
        command += str(duration)
        command += "\r\n"

        
        try:
            flage_ = serialPort.write(command)
        except expression,e:
            return False
        if flage_ > 0:
            return True
        else:
            return False
        

def main():
        
    portname = "/dev/servo_0"
    baudrate = 115200

    try:
        serialPort = serial.Serial(portname, baudrate)
        serialPort.writeTimeout = 2 #写超时
        print ("机械臂串口打开成功%s"%portname)
        print (",波特率为%s"%baudrate)

        
    except Exception as e:
        print ("机械臂串口被占用，请重新启动")
	print(e)
	exit(1)
    
    while True:
        w = read(1)
        print (w)
        time.sleep(1)

if __name__ == '__main__' :
    main()
