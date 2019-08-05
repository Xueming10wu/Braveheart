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
    //int position = protocol.readJoint(serialPort, 7);
    //cout << "position is :" << position << endl;

    protocol.writeJoint(serialPort, 7, 1500, 100);

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