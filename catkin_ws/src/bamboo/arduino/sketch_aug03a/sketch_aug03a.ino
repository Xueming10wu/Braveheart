#include <MotorProtocol.h>
#include <LiquidCrystal.h>


const uint8_t rs = 43 , rw = 41 , enable = 39 ,
              d0 = 37,  d1 = 35,  d2 = 33,  d3 = 31,
              d4 = 29,  d5 = 27,  d6 = 25,  d7 = 23;

LiquidCrystal lcd = LiquidCrystal( rs,  rw,  enable, d0,  d1,  d2,  d3, d4,  d5,  d6,  d7);


void setup()
{
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("OK");
}

int main(int argc, char *argv[])
{
  init();
  setup();
  MotorProtocol protocol = MotorProtocol();
  int left = 0;
  int right = 0;

  char * rxBuffer = new char[protocol.getSumSize()];
  bool canRead = false;

  int check = 0;

  while (true)
  {
      /*//写串口
      if (abs(left) > 0x7fff || abs(right) > 0x7fff)
      {
      left = 0;
      right = 0;
      }
      else
      {
      left ++;
      right ++;
      }
      protocol.setLeft(left);
      protocol.setRight(-right);
      if (protocol.encode())
      {
         if(Serial.availableForWrite() > protocol.getSumSize() * 2)
         {
           Serial.write(protocol.txBuffer, protocol.getSumSize());
         }

      }*/

    if (Serial.available())
    {
      int res_len = Serial.readBytes(rxBuffer, protocol.getSumSize());
      //cout << "res_len  " << res_len << endl;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("res_len:  ");
      Serial.read();
      /*
      if (res_len >= protocol.getSumSize())
      {
        for (size_t i = 0; i < protocol.getSumSize(); i ++)
        {
          //lcd.setCursor(0, 1);
          //lcd.print("real: ");
          //lcd.print(earth_val);
          canRead = protocol.decode(rxBuffer[i]);
          if (canRead)
          {
            //cout << "receive data  " << protocol.getLeft() << "  " << protocol.getRight() << endl;
          }
        }
      }*/
    }
    else
    { 
      check ++;
      if(check >= 100)
      {
        //lcd.setCursor(0, 0);
        //lcd.print("No data get");
      }

      //cout << "No data get" << endl;
    }
    //loopcount++;
    //cout << "loopcount  "<< loopcount << endl << endl;
    delay(10);
  }


  return 0;
}
