#include <MotorProtocol.h>
#include <Encoder.h>

MotorProtocol protocol = MotorProtocol();

//编码器引脚
const size_t Left_encode_A = 7;
const size_t Left_encode_B = 8;
const size_t Right_encode_A = 5;
const size_t Right_encode_B = 6;


//左速度控制引脚定义
Encoder knobLeft(Left_encode_A, Left_encode_B);
int PWMPinL = 10;
int OUTL1 = 11;
int OUTL2 = 12;
int fadeValueL = 0;      //占空比

//右速度控制引脚定义
Encoder knobRight(Right_encode_A, Right_encode_B);
int PWMPinR = 4;
int OUTR1 = 2;
int OUTR2 = 3;
int fadeValueR = 0;      //占空比


int *err = new int[12];
int left_set, right_set;
int last_left_set, last_right_set;
long newLeft = 0, newRight = 0;
size_t count;

/////////////////////////////////////读串口设置////////////////////////////////////////////////
//收数据缓存区
char *rxBuffer = new char[protocol.getSumSize() * 2];
//串口收缓存区长度
size_t res_len = 0;
//接收是否可以进行正常的读取
bool canRead = false;

/////////////////////////////////////读串口设置完毕////////////////////////////////////////////

void setup() {
  //Serial.flush();
  Serial.begin(9600);
  pinMode(OUTL1, OUTPUT);
  pinMode(OUTL2, OUTPUT);
  pinMode(OUTR1, OUTPUT);
  pinMode(OUTR2, OUTPUT);
  count = 0;
}

void loop()
{
  ////////////////////////////////////////////////////////10hz////////////////////////////////
  if (10 == count)
  {

    if (((last_left_set > 0) && (left_set < 0)) || ((last_left_set < 0) && (left_set > 0)))
    {
      left_set = 0;
    }
    if(((last_right_set > 0) && (right_set < 0)) || ((last_right_set < 0) && (right_set > 0)))
    {
      right_set = 0;
    }
    last_left_set = left_set;
    last_right_set = right_set;

          
    count = 0;
    newLeft = knobLeft.read();
    newRight = -knobRight.read();
    knobLeft.write(0);
    knobRight.write(0);


    //////////////////////////////////////写串口/////////////////////////
    protocol.setLeft(newLeft);
    protocol.setRight(newRight);
    if (protocol.encode())
    {
      if (Serial.availableForWrite() > protocol.getSumSize() * 2)
      {
        Serial.write(protocol.txBuffer, protocol.getSumSize());
      }
    }
    //////////////////////////////////////写串口完毕//////////////////////

    //left_set = 200;
    /////////////////////////////////////速度控制pid/////////////////////
    if (left_set >= 0)
    {
      err[0] = left_set - newLeft;
      fadeValueL = fadeValueL + 0.08 * err[0] + 0.12 * (err[0] - err[1]);    //0.08+0.09
      err[1] = err[0];
      err[2] = err[1];
      if (fadeValueL >= 255)
      {
        fadeValueL = 255;
      }
      if (fadeValueL <= 0)
      {
        fadeValueL = 0;
      }
      digitalWrite(OUTL1, HIGH);
      digitalWrite(OUTL2, LOW);
      analogWrite(PWMPinL, fadeValueL);
      //    //Serial.println(newLeft);
      //    Serial.println(newLeft);
    }
    ///////////////////////////err 3 4 5
    else
    {
      err[3] = left_set - newLeft;
      err[3] = -err[3];
      fadeValueL = fadeValueL + 0.08 * err[3] + 0.12 * (err[3] - err[4]);
      err[4] = err[3];
      err[5] = err[4];
      if (fadeValueL >= 255)
      {
        fadeValueL = 255;
      }
      if (fadeValueL <= 0)
      {
        fadeValueL = 0;
      }
      digitalWrite(OUTL1, LOW);
      digitalWrite(OUTL2, HIGH);
      analogWrite(PWMPinL, fadeValueL);
      //    Serial.println(newLeft);
      //    Serial.println(err[3]);
    }
    /////////////////////////// 右轮 ///////////////////////
    //right_set = 200;
    ///////////////////////////err 6 7 8
    if (right_set >= 0)
    {
      err[6] = right_set - newRight;
      fadeValueR = fadeValueR + 0.08 * err[6] + 0.12 * (err[6] - err[7]);    //0.08+0.09
      err[7] = err[6];
      err[8] = err[7];
      if (fadeValueR >= 255)
      {
        fadeValueR = 255;
      }
      if (fadeValueR <= 0)
      {
        fadeValueR = 0;
      }
      digitalWrite(OUTR1, HIGH);
      digitalWrite(OUTR2, LOW);
      analogWrite(PWMPinR, fadeValueR);
      //Serial.println(newLeft);
      //    Serial.println(newRight);
      //    Serial.println();
    }
    ///////////////////////////err 9 10 11
    else
    {
      err[9] = right_set - newRight;
      err[9] = -err[9];
      fadeValueR = fadeValueR + 0.08 * err[9] + 0.12 * (err[9] - err[10]);
      err[10] = err[9];
      err[11] = err[10];
      if (fadeValueR >= 255)
      {
        fadeValueR = 255;
      }
      if (fadeValueR <= 0)
      {
        fadeValueR = 0;
      }
      digitalWrite(OUTR1, LOW);
      digitalWrite(OUTR2, HIGH);
      analogWrite(PWMPinR, fadeValueR);
      //    Serial.println(newRight);
      //    Serial.println(err[9]);
      //    Serial.println();
    }
    /////////////////////////////////////pid结束/////////////////////////
  }
  ////////////////////////////////////////////////////////////10 hz结束 ///////////////////////

  //////////////////////////////////////读串口(100hz)//////////////////
  if (Serial.available() > protocol.getSumSize())
  {
    res_len = Serial.readBytes(rxBuffer, protocol.getSumSize());

    if (res_len >= protocol.getSumSize())
    {
      for (size_t i = 0; i < protocol.getSumSize(); i ++)
      {
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
  //////////////////////////////////////读串口结束//////////////////////

  delay(10);
  count++;
}
