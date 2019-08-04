#include "bamboo/ArmProtocol.h"

//构造函数
ArmProtocol::ArmProtocol(){
    cout << "ArmProtocol construct" << endl;
}

//析构函数
ArmProtocol::~ArmProtocol(){}


//读取某个舵机的脉宽(角度)，返回0则是读取数据失败
int ArmProtocol::readJoint( int joint_id)
{
    //  #1PRAD\r\n
    stringstream ss;
    ss << joint_id;
    string joint_id_str = ss.str();
    char *joint_id_chr = (char *)joint_id_str.c_str();
    int id_len = strlen(joint_id_chr);


    char *request = new char[ 8 + id_len ];
    request[ 8 + id_len - 1] = '\0';

    request[0] = '#';
    for (size_t i = 0; i < strlen(joint_id_chr); i++)
    {
        request[1 + i] = joint_id_chr[i];
    }
    request[1 + id_len] = 'P';
    request[2 + id_len] = 'R';
    request[3 + id_len] = 'A';
    request[4 + id_len] = 'D';
    request[5 + id_len] = '\r';
    request[6 + id_len] = '\n';
    
    cout << request << strlen(request) << endl;
    //写入
    //serialport.write()


    //响应
    char *response = new char [11];
    int key = 0;
    
    while(true)
    {   
        //serialport.available() > 0
        if (true)
        {
            //数据读取
            //response[key] = (serialport.read(1).c_str())[0];
            key ++;
            if (response[0] != '#')
            {//丢包错误
                key = 0;
                continue;
            }
            if (key < 11)
            {
                //正常执行时
                continue;
            }
            else
            {   //当首位为#，并且key为11时，说明数据接收完毕
                break;
            }
        }
        else
        {
            usleep(10);
        }
    }

    //串口写入 串口读出
    char *joint_res = new char[3];
    char *position_res = new char[4];


    

    
    return 0;
}

//写某个舵机的脉宽(角度)  返回是否成功 成功:1  失败:0
int ArmProtocol::writeJoint(Serial& sp, int joint, int puls_width, int duration )
{

    return 0;
}