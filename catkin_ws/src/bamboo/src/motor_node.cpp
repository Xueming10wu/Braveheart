#include "bamboo/MotorProtocol.h"


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
    int baudrate = 9600;
    const string port = "/dev/ttyUSB0";
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

    //实例化协议对象
    MotorProtocol protocol = MotorProtocol();

/////////////////////////////////////读串口设置////////////////////////////////////////////////
    //收数据缓存区
    uint8_t *rxBuffer = new uint8_t[protocol.getSumSize() * 2];
    //串口收缓存区长度
    size_t res_len = 0;
    //接收是否可以进行正常的读取
    bool canRead = false;
/////////////////////////////////////读串口设置完毕////////////////////////////////////////////


    while (!quit_flag)
    {
//////////////////////////////////////写串口///////////////////////////////////////////////////////
        //写串口
        //将数据封装到通讯协议里面
        protocol.setLeft(-15);
        protocol.setRight(95);

        //协议编码
        if (protocol.encode())
        {   
            //检查串口数据输出缓存区状态
            int txnum = serialPort.write(protocol.txBuffer, protocol.getSumSize());
            //将协议封装好的数据写入串口中
            cout << "wrote :" << txnum << endl;
        }
        else
        {
            cout << "ENCODE FAILED: left or right value is too large for 16 bit" << endl;
            return 1;
        }
//////////////////////////////////////写串口完毕///////////////////////////////////////////////////
        


        
//////////////////////////////////////读串口////////////////////////////////////////////////////////
        /*
        //检查串口接收缓存区中数据的大小，判断是否能够使用协议进行解码
        if (serialPort.available() > protocol.getSumSize())
        {  
            //读取协议封装长度的数据，并返回读取的数据长度
            res_len = serialPort.read(rxBuffer, protocol.getSumSize());

            //如果读取的数据长度多余协议进行解码所需要的最短长度，那就尝试进行解码
            if (res_len >= protocol.getSumSize())
            {
                for (size_t i = 0; i < protocol.getSumSize(); i ++)
                {   
                    //将uint8_t类型的数据保存到协议对象中
                    //查看是否解码已经成功
                    canRead = protocol.decode(rxBuffer[i]);
                    if (canRead)
                    {
                        //读取解码后的具体数据
                        cout << "receive data  " << protocol.getLeft() << "  " << protocol.getRight() << endl;
                    }
                }
            }
        }
        else
        {
            //cout << "No data get" << endl;
        }*/
//////////////////////////////////////读串口结束//////////////////////////////////////////////////////
        
        loopcount++;
        cout << "loopcount  "<< loopcount << endl << endl;
        usleep(100000);
        
    }
    
    return 0;
}
