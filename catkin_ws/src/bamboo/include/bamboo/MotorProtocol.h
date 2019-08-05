#ifndef MOTORPROTOCOL
#define MOTORPROTOCOL

#include "bamboo/common.h"
//底盘通讯库

class MotorProtocol
{
    public:
        MotorProtocol();
        ~MotorProtocol();

        //编码
        bool encode();

        //解码
        bool decode( uint8_t unData );

		//设置
		void setLeft( int value );
		void setRight( int value );

		//获取
		int getLeft() const;
		int getRight() const;
		int getSumSize() const;

		//编码后的缓存区
		uint8_t* txBuffer;

    private:
		//左右轮的速度
        int left;
        int right;

		//数据段长度
		const int start_size;
        const int left_H_size;
        const int left_L_size;
        const int right_H_size;
        const int right_L_size;
        const int stop_size;

        const int sum_size;

		//接收缓存
		uint8_t *rxBuffer;

		//接收缓存中位置信息
		int rxIndex;
};

#endif //NEWMOTORPROTOCOL