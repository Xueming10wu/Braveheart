#include "bamboo/ArmProtocol.h"


//使用ctrl+C可以退出程序
int quit_flag = 0;
void exit_handler(int sig)
{
    quit_flag = 1;
}

//记录循环次数
int loopcount = 0;


int main(int argc, char *argv[])
{
    //按键捕获，用于退出while循环
    signal(SIGINT,exit_handler);

    //串口设置
    int baudrate = 115200;
    const string port = "/dev/ttyUSB1";
    Timeout timeout = Timeout::simpleTimeout(1000);
    Serial serialPort(port, baudrate, timeout);

    cout << "Is the serial port open?";
    if(serialPort.isOpen())
    {
        cout << " Yes." << endl;
    }
    else
    {
        cout << " No." << endl;
        return 1;
    }
    

    //cout << "Hello arm" << endl;
    ArmProtocol protocol = ArmProtocol();

    usleep(1000000);

    //读取舵机测试
    //int position = protocol.readJoint(serialPort, 7);
    //cout << "position is :" << position << endl;


    //串口实例  舵机号  脉宽  时间 
    protocol.writeJoint(serialPort, 7, 1500, 100);

    //串口实例  舵机号  脉宽  时间 
    protocol.writeJoint(serialPort, 7, 1500, 100);
    
    //写多个舵机的脉宽(角度)  返回是否成功    参数: 串口实例  数组长度  舵机号组  脉宽组  时间 
    int count = 7;
    int* joint_unit = new int[count];
    int* width_unit = new int[count];
    int duration = 500;

    for (size_t i = 1; i <= count; i++)
    {
        joint_unit[i] = i;
    }

    for (size_t i = 0; i < count; i++)
    {
        width_unit[i] = 1500;
    }
    
    protocol.writeMulJoint(serialPort , count, joint_unit, width_unit, duration );


    /*
    while (true)
    {
        int position = protocol.readJoint(serialPort, 263);
        cout << "position is :" << position << endl;

        usleep(1000000);
    }
    */

    return 0;
}