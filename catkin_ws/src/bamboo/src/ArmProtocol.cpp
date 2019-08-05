#include "bamboo/ArmProtocol.h"


//构造函数
ArmProtocol::ArmProtocol():bufferSize(1024)
{
    rxBuffer = new uint8_t[bufferSize];
    txBuffer = new uint8_t[bufferSize];
    rxBuffer[bufferSize - 1] = '\0';
    txBuffer[bufferSize - 1] = '\0';
    KeyRX = 0;
    KeyTX = 0;
    cout << "ArmProtocol construct" << endl;
}


//析构函数
ArmProtocol::~ArmProtocol()
{
    delete []txBuffer;
    delete []rxBuffer;
    txBuffer = NULL;
    rxBuffer = NULL;
}


//读取某个舵机的脉宽(角度)，返回0则是读取数据失败
int ArmProtocol::readJoint(Serial &serialport, int joint_id)
{
    /////////////////////////////////////请求数据消息的创建////////////////////////////////////////
    stringstream ss;
    ss << "#";
    ss << joint_id << "PRAD\r\n";

    string txstring = ss.str();
    char *tx_char = (char *)txstring.c_str();
    KeyTX = strlen(tx_char);

    cout << "len " << KeyTX << " " << txstring;
    for (size_t i = 0; i < KeyTX; i++)
    {
         txBuffer[i] = (uint8_t)tx_char[i];
    }
    /////////////////////////////////////请求数据消息的创建完毕//////////////////////////////////////


    //////////////////////////////////////串口请求发送/////////////////////////////////////////////////
    int txnum = serialport.write(txBuffer, KeyTX);
    if (txnum != KeyTX)
    {
        cout << "readJoint write failed" << endl;
        return 0;
    }
    else
    {
        cout << "readJoint write successed" << endl;
    }

    //////////////////////////////////////串口请求发送完毕/////////////////////////////////////////////


    /////////////////////////////////////响应数据的接收//////////////////////////////////////////
    //固定位数为11位
    KeyRX = 11;

    //rx索引
    int key = 0;
    bool canRead = false;
    
    
    while(!canRead)
    {   
        //串口是否有数据
        if (serialport.available())
        {
            //数据读取
            rxBuffer[key] = (serialport.read(1).c_str())[0];
            key += 1;
            if (rxBuffer[0] != (uint8_t)'#')
            {   
                //丢包错误
                cout << "lost package:  #" << endl;
                key = 0;
                continue;
            }
            if (key < 11)
            {
                //正常执行时
                continue;
            }
            if (rxBuffer[9] != (uint8_t)'\r')
            {
                cout << "lost package:  \\r" << endl;
                key = 0;
                continue;
            }
            if (rxBuffer[10] != (uint8_t)'\n')
            {
                cout << "lost package:  \\n" << endl;
                key = 0;
                continue;
            }
            //通过检测，可以读取数据
            canRead = true;
        }
        else
        {   
            //等待数据
            usleep(2000);
            cout << "No data get" << endl;
        }
    }

    cout << "RXBUFFER ";
    for (size_t i = 0; i < KeyRX; i++)
    {
        cout << int(rxBuffer[i]) << " ";
    }
    cout << "\n\n";
    

    
    /////////////////////////////////////响应数据的接收完毕//////////////////////////////////////////



    /////////////////////////////////////响应数据的解码////////////////////////////////////////////

    //串口读出
    int res_joint_id = int((rxBuffer[1] - uint8_t('0'))* 100 
                    + (rxBuffer[2] - uint8_t('0')) * 10 
                    + (rxBuffer[3] - uint8_t('0')) * 1);
                    
    int res_position = int((rxBuffer[5] - uint8_t('0') )* 1000 
                    + (rxBuffer[6] - uint8_t('0') ) * 100 
                    + (rxBuffer[7] - uint8_t('0')) * 10 
                    + (rxBuffer[8] - uint8_t('0')) * 1);
    /////////////////////////////////////响应数据的解码完毕//////////////////////////////////////////

    if (res_joint_id == joint_id)
    {
        return res_position;
    }
    else
    {
        cout << "Read except joint is " << joint_id << " , but respond " << res_joint_id << " , position " << res_position << endl;
    }
    
    return 0;
}


//写某个舵机的脉宽(角度)  返回是否成功 成功:1  失败:0
int ArmProtocol::writeJoint( Serial &serialport, int joint_id, int puls_width, int duration )
{
    stringstream ss;

    ss << "#";
    ss << joint_id << "P" << puls_width;
    ss << "T" << duration << "\r\n" ;
    string txstring = ss.str();

    char *tx_char = (char *)txstring.c_str();
    KeyTX = strlen(tx_char);
    cout << "len " << KeyTX << " " << txstring;

    for (size_t i = 0; i < KeyTX; i++)
    {
         txBuffer[i] = (uint8_t)tx_char[i];
    }

    int txnum = serialport.write(txBuffer, KeyTX);
    if (txnum != KeyTX)
    {
        cout << "writeJoint write failed :" << endl;
        return 0;
    }
    else
    {
        cout << "writeJoint write success !" << endl;
    }
    return 1;
}


void ArmProtocol::flushTxBuffer()
{
    for (size_t i = 0; i < bufferSize - 1; i++)
    {
        txBuffer[i] = '$';
    }
    KeyTX = 0;
}

void ArmProtocol::flushRxBuffer()
{
    for (size_t i = 0; i < bufferSize - 1; i++)
    {
        rxBuffer[i] = '$';
    }
    KeyRX = 0;
}

void ArmProtocol::flush()
{
    flushTxBuffer();
    flushRxBuffer();
}