#include "MotorProtocol.h"

MotorProtocol::MotorProtocol(): start_size(1), left_H_size(1), left_L_size(1), right_H_size(1), right_L_size(1), stop_size(1), sum_size(6)
{
    //cout << "construction" << endl;
    //内存分配
    txBuffer = new uint8_t[sum_size * 2];
    rxBuffer = new uint8_t[sum_size * 2];
    rxIndex = 0;
    left = 0;
    right =0;
}

MotorProtocol::~MotorProtocol()
{
    delete txBuffer;
}

bool MotorProtocol::encode()
{
     //cout << "encode left right: " <<  left << " " << right << endl;
     //数据不超过15位
     if(abs(left) <  0x7fff && abs(right) < 0x7fff)
     {
        //设置符号位
        int left_signed,right_signed;
        if( left > 0 )
        {
            left_signed = 0;
        }
        else
        {
            left_signed = 1;
        }

        if( right > 0)
        {
            right_signed = 0;
        }
        else
        {
            right_signed = 1;
        }

        //高8位，低8位
        int unsigned left_H,left_L,right_H,right_L = 0;

        //开始标识 '$'
        char startFlag = '$';

        //结束标识 '!'
        char stopFlag = '!';

        //高8位赋值
        left_H = abs(left);
        right_H = abs(right);
        left_H = ((left_H >> 8) & 0x7f)| (left_signed << 7);
        right_H = ((right_H >> 8) & 0x7f )| (right_signed << 7);

        //低8位赋值
        left_L = abs(left);
        right_L = abs(right);
        left_L &= 0xff;
        right_L &= 0xff;


        //写入缓存中
        txBuffer[0] = uint8_t(startFlag);
        txBuffer[1] = uint8_t(left_H);
        txBuffer[2] = uint8_t(left_L);
        txBuffer[3] = uint8_t(right_H);
        txBuffer[4] = uint8_t(right_L);
        txBuffer[5] = uint8_t(stopFlag);
        return true;
     }
     else
     {
         //std::cout << "encode : arg is too long\n";
         return false;
     }
     return true;
}

bool MotorProtocol::decode( uint8_t unData )
{
    //cout << "decode()   :  rxIndex " << rxIndex << "  unData " << (int)unData << endl;
    rxBuffer[rxIndex] = unData;
    rxIndex += 1;

    if (rxBuffer[0] != '$')
    {
        //cout << "no right start $" << endl;
        //如果发现第一个数据不是界符的话，就将索引置0
        rxIndex = 0;
        return false;
    }

    if (rxIndex < sum_size )
    {
        //记录数组位置
        //如果数据索引没有到11的话，退出并再次执行本函数
        return false;
    }

    if( rxBuffer[sum_size - 1] != '!')
    {
        //cout << "no right end !"  << endl;
        rxIndex = 0;
        return false;
    }


    //通过丢包检测
    //设置符号位
    int left_signed;int right_signed = 0;
    int left_H = 0;int unsigned left_L = 0;
    int right_H = 0; int unsigned right_L = 0;


    //4个字段获取数据
    left_H = int(rxBuffer[1]) & 0xff;left_L = int(rxBuffer[2]) & 0xff;
    right_H = int(rxBuffer[3]) & 0xff;right_L = int(rxBuffer[4]) & 0xff;

    //清理数据
    rxIndex = 0;
    for (size_t i = 0; i < sum_size; i++)
    {
        rxBuffer[i] = '@';
    }

    //符号获取
    left_signed = left_H >> 7;right_signed = right_H >> 7;

    //根据正负号进行赋值
    if (left_signed > 0)
    {
        left = (-1) * (((left_H & 0x7f) << 8) | left_L);
    }
    else
    {
        left = ((left_H & 0x7f) << 8) | left_L;
    }
        
    if (right_signed > 0)
    {
        right = (-1) * (((right_H & 0x7f) << 8) | right_L);
    }
    else
    {
        right = ((right_H & 0x7f) << 8) | right_L;
    }

    return true;
}



//设置
void MotorProtocol::setLeft( int value ){left = value;}
void MotorProtocol::setRight( int value ){right = value;}

//获取
int MotorProtocol::getLeft() const{return left;}
int MotorProtocol::getRight() const{return right;}
int MotorProtocol::getSumSize() const{return sum_size;}
