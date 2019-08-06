#include "bamboo/BambooMotor.h"

BambooMotor::BambooMotor():PI(3.1415926)
{
    //私有参数 获取
    ros::NodeHandle nh_private("~");
    nh_private.param<string>("serialport_name", serialport_name, "/dev/arduino_due");
    nh_private.param<int>("baudrate", baudrate, 9600);
    nh_private.param<int>("reduction" ,reduction , 36);
    nh_private.param<double>("wheel_radius" ,wheel_radius , 0.05);
    nh_private.param<double>("base_width" ,base_width , 0.30);
    nh_private.param<int>("wheel_left_direction" ,wheel_left_direction , 1 );
    nh_private.param<int>("wheel_right_direction" ,wheel_right_direction , 1 );
    nh_private.param<int>("encoder_left_direction" ,encoder_left_direction , 1 );
    nh_private.param<int>("encoder_right_direction" ,encoder_right_direction , 1 );
    nh_private.param<int>("PPR" ,PPR , 11 );
    nh_private.param<int>("QPR" ,QPR , PPR * 4);

    //串口初始化
    Timeout timeout = Timeout::simpleTimeout(1000);
    serialPort = new Serial(serialport_name, baudrate, timeout);

    //检查串口是否正常
    if(serialPort->isOpen()){cout << " serialPort isOpen." << endl;}
    else{cout << " serialPort Opened failed" << endl;return;}

    //通信协议初始化
    protocol = new MotorProtocol();

    //休眠3s,丢弃掉串口中错误的信息
    usleep(3000000);
    serialPort->flush ();

    //设置一些初始值

    //发布者 接收者 初始化
    pub_odom = nh.advertise<nav_msgs::Odometry>("odom", 10);
    sub_speed = nh.subscribe("/cmd_vel", 10, &BambooMotor::cmdVelCallBack, this);

    // ros 时钟控制
    ros::Rate loop_rate(10);
    time_prev = ros::Time::now();
    time_current = ros::Time::now();

    //ros消息 坐标系等初始化
    tf_transform.header.frame_id = "odom";
    tf_transform.child_frame_id = "base_link";
    odom.header.frame_id = "odom";
    odom.child_frame_id= "base_link";


    //里程计和姿态积分   长度单位 m  角度为 弧度角 即 旋转3.14为旋转半圈
    //单位时间从下位机读取编码器的数据  数据为 QPR，即PPR*4
    dleft_encoder = 0; dright_encoder = 0;
    //单位时间下机器人的姿态变化  坐标系为 前一时刻机器人坐标系
    dx = 0; dy = 0; dtheta = 0;
    //单位时间下机器人中心在x方向上移动的距离，单位时间下机器人的左右轮子移动的距离
    d = 0; dleft = 0; dright = 0;
	//初始位置下 编码器的数据
	left_encoder = 0; right_encoder = 0;
	//初始位置下 机器人的姿态
	tf_x = 0; tf_y = 0; tf_theta = 0;
	//初始位置下 机器人中心在x方向上移动的距离，机器人的左右轮子移动的距离
	tf_distance = 0; tf_left = 0; tf_right = 0;


    //机器人当前的位置和姿态信息  相对于初始位置
    tf_x = 0;
    tf_y = 0;
    tf_theta = 0;


    //读串口设置
    //收数据缓存区
    uint8_t *rxBuffer = new uint8_t[protocol->getSumSize() * 2];
    //串口收缓存区长度
    size_t res_len = 0;
    //接收是否可以进行正常的读取
    bool canRead = false;


    while (nh.ok())
    {
        ///////////////////////////////读串口///////////////////////////////
        //检查串口接收缓存区中数据的大小，判断是否能够使用协议进行解码
        if (serialPort->available() > protocol->getSumSize())
        {  
            //读取协议封装长度的数据，并返回读取的数据长度
            res_len = serialPort->read(rxBuffer, protocol->getSumSize());

            //如果读取的数据长度多余协议进行解码所需要的最短长度，那就尝试进行解码
            if (res_len >= protocol->getSumSize())
            {
                for (size_t i = 0; i < protocol->getSumSize(); i ++)
                {   
                    //将uint8_t类型的数据保存到协议对象中
                    //查看是否解码已经成功
                    canRead = protocol->decode(rxBuffer[i]);
                    if (canRead)
                    {
                        //读取解码后的具体数据，解析完成后才能发送数据
                        publishOdom();
                        cout << "receive data  " << protocol->getLeft() << "  " << protocol->getRight() << endl;
                    }
                }
            }
        }
        else
        {
            //cout << "No data get" << endl;
        }
        ///////////////////////////////读串口结束///////////////////////////////
        ros::spinOnce();
        loop_rate.sleep();
    }
}

BambooMotor::~BambooMotor()
{}


//回调函数
void BambooMotor::cmdVelCallBack(const geometry_msgs::Twist & msg)
{
    //根据线速度设置 左右的速度
    double speed_l = wheel_left_direction * msg.linear.x;
    double speed_r = wheel_right_direction * msg.linear.x;


    //加上旋转的速度    线速度 = 角速度 * 半径， 然后分摊到左右两边
    speed_l += (wheel_left_direction * msg.angular.z * base_width/2) / 2;
    speed_r -= (wheel_right_direction * msg.angular.z * base_width/2) / 2;
    ROS_INFO("Set speed left: %f, right: %f", speed_l, speed_r);


    //速度转化为目标脉冲数
    int left_except_pulse = encoder_left_direction * round(speed_l / QPR); 
    int right_except_pulse = encoder_right_direction * round(speed_r / QPR);
    
    //写串口
    //将数据封装到通讯协议里面
    protocol->setLeft(left_except_pulse);
    protocol->setRight(right_except_pulse);

    //协议编码
    if (protocol->encode())
    {   
        //检查串口数据输出缓存区状态
        int txnum = serialPort->write(protocol->txBuffer, protocol->getSumSize());
        //将协议封装好的数据写入串口中
        cout << "wrote :" << txnum << endl;
    }
    else
    {
        cout << "ENCODE FAILED: left or right value is too large for 16 bit" << endl;
        return;
    }
}//回调函数结束


//发布者发布函数
void BambooMotor::publishOdom()
{   
    //里程计和姿态积分   长度单位 m
    //获取协议解码后的数据
    dleft_encoder = encoder_left_direction * protocol->getLeft();
    dright_encoder = encoder_right_direction * protocol->getRight();

    //里程计累加
    left_encoder += dleft_encoder;
    right_encoder += dright_encoder;

    //单位时间下机器人中心在x方向上移动的距离，单位时间下机器人的左右轮子移动的距离
    dleft = wheel_left_direction * dleft_encoder * reduction / QPR;
    dright = wheel_left_direction * dright_encoder * reduction / QPR;
    d = (dleft + dright) / 2;

    //轮子运动的距离
    tf_left = wheel_left_direction * left_encoder * reduction / QPR;
    tf_right = wheel_left_direction * right_encoder * reduction / QPR;
    tf_distance = (tf_left + tf_right)/2;

    //转角  z轴正方向
    dtheta = (dright - dleft) / base_width;
    tf_theta += dtheta;
    
    //有数据时候才能进行角度变换
    if (d != 0)
    {
        dx = cos(dtheta) * d;
        dy = sin(dtheta) * d;
        tf_x += dx*cos(tf_theta) - dy*sin(tf_theta);
        tf_y += dx*sin(tf_theta) + dy*cos(tf_theta);
    }
    
    
    
    //单位时间从下位机读取编码器的数据  数据为 QPR，即PPR*4
    int dleft_encoder, dright_encoder;
    //单位时间下机器人的姿态变化  坐标系为 前一时刻机器人坐标系
    double dx, dy, dtheta;
    //单位时间下机器人中心在x方向上移动的距离，单位时间下机器人的左右轮子移动的距离
    double d, dleft, dright;
	//初始位置下 编码器的数据
	long long left_encoder, right_encoder;
	//初始位置下 机器人的姿态
	long double tf_x, tf_y, tf_theta;
	//初始位置下 机器人中心在x方向上移动的距离，机器人的左右轮子移动的距离
	long double tf_distance, tf_left, tf_right;

    //获取时间
    time_current = ros::Time::now();
    double elapsed = (time_current - time_prev).toSec();
    time_prev = time_current;

    // 发布 tf 信息
    geometry_msgs::Quaternion odom_quaternion = tf::createQuaternionMsgFromYaw(tf_theta);
    tf_transform.header.stamp = time_current;
    tf_transform.transform.translation.x = tf_x;
    tf_transform.transform.translation.y = tf_y;
    tf_transform.transform.rotation = odom_quaternion;
    tf_broadcaster.sendTransform(tf_transform);

    // 发布里程计信息
    odom.header.stamp = time_current;
    odom.pose.pose.position.x = tf_x;
    odom.pose.pose.position.y = tf_y;
    odom.pose.pose.orientation = odom_quaternion;
    odom.twist.twist.linear.x = d / elapsed;
    odom.twist.twist.angular.z = dtheta / elapsed;
    pub_odom.publish(odom);


}//发布者函数结束