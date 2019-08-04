#include <MotorProtocol.h>
#include <Encoder.h>

//协议实例化
MotorProtocol protocol = MotorProtocol();   


//编码器引脚
const size_t Left_encode_A = 7;
const size_t Left_encode_B = 8;
const size_t Right_encode_A = 5;
const size_t Right_encode_B = 6;


//左边编码器对象
Encoder knobLeft(Left_encode_A, Left_encode_B);
//左侧电机控制引脚
const int PWMPinL = 10;
//左侧电机 使能端1
const int OUTL1 = 11;
//左侧电机 使能端2
const int OUTL2 = 12;
int fadeValueL;      //占空比


//右边编码器对象
Encoder knobRight(Right_encode_A, Right_encode_B);
//右侧电机控制引脚
const int PWMPinR = 4;
//右侧电机 使能端1
const int OUTR1 = 2; 
//右侧电机 使能端2           
const int OUTR2 = 3;
int fadeValueR;      //占空比


//PID参数组
long positionLeft  = -999;
long positionRight = -999;
int *err = new int[3];
int left_set,right_set;
long newLeft, newRight;

//设置一个计数变量，每循环10次，发一次编码器的数据
size_t count = 0;

void setup() 
{
  //Arduino DUE 工作方式设置
  pinMode(OUTL1, OUTPUT);     
  pinMode(OUTL2, OUTPUT);
  pinMode(OUTR1, OUTPUT);     
  pinMode(OUTR2, OUTPUT);
  Serial.begin(9600);
  //Serial.println("Encoder Test:");
}

/////////////////////////////////////读串口设置////////////////////////////////////////////////
//收数据缓存区
char *rxBuffer = new char[protocol.getSumSize() * 2];
//串口收缓存区长度
size_t res_len = 0;
//接收是否可以进行正常的读取
bool canRead = false;

/////////////////////////////////////读串口设置完毕////////////////////////////////////////////


void loop() 
{
  if(10 == count)
  { 
    //符合发送编码器数据的条件
    count = 0;
    
    //获得编码器数据
    newLeft = knobLeft.read();
    newRight = knobRight.read();

    //编码器数据清零
    knobLeft.write(0);
    knobRight.write(0);

//////////////////////////////////////写串口///////////////////////////////////////////////////////
    //将数据封装到通讯协议里面
    protocol.setLeft(newLeft);
    protocol.setRight(newRight);

    //协议编码
    if (protocol.encode())
    {   
        //检查串口数据输出缓存区状态
        if(Serial.availableForWrite() > protocol.getSumSize() * 2)
        {
          //将协议封装好的数据写入串口中
          Serial.write(protocol.txBuffer, protocol.getSumSize());
        }
    }
//////////////////////////////////////写串口完毕///////////////////////////////////////////////////


  
/////////////////////////////////////速度控制pid//////////////////////////////////////////////////
  /*
  if(protocol.left_vel >= 0)
  {
    err[1] = protocol.left_vel - newLeft;
    fadeValueL += 0.3*err[1];
    if(fadeValueL >= 255)
    {
      fadeValueL = 255;
    }
    digitalWrite(OUTL1, LOW);   
    digitalWrite(OUTL2, HIGH);
    analogWrite(PWMPinL,fadeValueL);
  }
  else
  {
    
  }
  if(protocol.right_vel >= 0)
  {
    err[1] = protocol.right_vel - newRight;
    fadeValueR += err[1];
    if(fadeValueR >= 255)
    {
      fadeValueR = 255;
    }
    digitalWrite(OUTR1, LOW);   
    digitalWrite(OUTR2, HIGH);
    analogWrite(PWMPinR,fadeValueR);
  }
  else
  {
    
  }*/
    err[0] = left_set - newLeft;
    fadeValueL = fadeValueL + 0.06*err[0] + 0.3*(err[0]-err[1]);
    err[1] = err[0];
    err[2] = err[1];
    if(fadeValueL >= 255)
    {
      fadeValueL = 255;
    }
    if(fadeValueL <= 0)
    {
      fadeValueL = 0;
    }
    digitalWrite(OUTL1, HIGH);   
    digitalWrite(OUTL2, LOW);
    analogWrite(PWMPinL,fadeValueL);
    //Serial.println(fadeValueL);
    //Serial.println(err[1]);
    
  }
 /////////////////////////////////////速度控制pid结束//////////////////////////////////////////////////   


//////////////////////////////////////读串口////////////////////////////////////////////////////////
    //检查串口接收缓存区中数据的大小，判断是否能够使用协议进行解码
    if (Serial.available() > protocol.getSumSize())
    {   
        //读取协议封装长度的数据，并返回读取的数据长度
        res_len = Serial.readBytes(rxBuffer, protocol.getSumSize());

        //如果读取的数据长度多余协议进行解码所需要的最短长度，那就尝试进行解码
        if (res_len >= protocol.getSumSize())
        {
            for (size_t i = 0; i < protocol.getSumSize(); i ++)
            {   
                //将uint8_t类型的数据保存到协议对象中
                //查看是否解码已经成功
                canRead = protocol.decode((uint8_t)rxBuffer[i]);
                if (canRead)
                {
                    //读取解码后的具体数据
                   left_set = protocol.getLeft();
                   right_set = protocol.getRight();
                }
            }
        }
    }
//////////////////////////////////////读串口结束//////////////////////////////////////////////////////
  
  delay(10);
  count++;

}
