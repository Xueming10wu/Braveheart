#ifndef ARMPROTOCOL
#define ARMPROTOCOL

#include "bamboo/common.h"

//机械臂通讯库
class ArmProtocol
{
public:
	ArmProtocol();
	~ArmProtocol();

	//读取某个舵机的脉宽(角度)  返回是否成功
	int readJoint(Serial & sp, int joint_id);

	//写某个舵机的脉宽(角度)  返回是否成功。  参数: 串口实例  舵机号  脉宽  时间 
	int writeJoint(Serial & sp, int joint_id, int puls_width, int duration );

	//写多个舵机的脉宽(角度)  返回是否成功    参数: 串口实例  数组长度  舵机号组  脉宽组  时间 
	int writeMulJoint(Serial & sp, int count, int* joint_id, int* puls_width, int duration );

private:
	//缓存数组最大长度
	const int bufferSize;

	//发送缓存
	uint8_t * txBuffer;

	//接收缓存
	uint8_t * rxBuffer;

	//记录缓存区中已用的位数
	int KeyRX;
	int KeyTX;

	//清理缓存
	void flushTxBuffer();
	void flushRxBuffer();
	void flush();

};

#endif // ArmProtocol