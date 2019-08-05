#ifndef BAMBOOMOTOR
#define BAMBOOMOTOR

#include "bamboo/MotorProtocol.h"


class BambooMotor
{
public:
	BambooMotor();
	virtual ~BambooMotor();

	//回调函数
	void cmdVelCallBack(const geometry_msgs::Twist & msg);

	//发布者发布函数
	void publishOdom();

private:
	//常量设置
	const double PI;

	//ros交互
	ros::NodeHandle nh;
	ros::Publisher pub_odom;
    ros::Subscriber sub_speed;
	tf::TransformBroadcaster tf_broadcaster;

	//ros消息
	geometry_msgs::TransformStamped tf_transform;
    nav_msgs::Odometry odom;

	//ros系统时间
	ros::Time time_current, time_prev;

	//串口交互
	Serial * serialPort;
	string serialport_name;
    int baudrate;

	//通信协议
	MotorProtocol * protocol;

	//PID积分


	//物理尺寸					 长度单位 m   
	int reduction;				//电机减速比
	double wheel_radius;			//轮子半径
	double base_width;				//轮子间距
	int left_direction;			//轮子方向，用于调整正反转
	int right_direction;		//轮子方向，用于调整正反转

	int PPR;					//编码器线数
	int QPR;					//PPR*4  编码器转一圈获得脉冲数量
};

#endif //BAMBOOMOTOR