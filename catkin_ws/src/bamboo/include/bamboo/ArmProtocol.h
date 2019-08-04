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
	int readJoint( int joint_id);

	//写某个舵机的脉宽(角度)  返回是否成功
	int writeJoint(Serial& sp, int joint, int puls_width, int duration );

private:

};

#endif // ArmProtocol