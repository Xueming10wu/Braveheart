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
    nh_private.param<int>("left_direction" ,left_direction , 1 );
    nh_private.param<int>("right_direction" ,right_direction , 1 );
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

    //PID积分初始化





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
                        //读取解码后的具体数据
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


        publishOdom();
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
    double speed_l = left_direction * msg.linear.x;
    double speed_r = right_direction * msg.linear.x;


    //加上旋转的速度    线速度 = 角速度 * 半径， 然后分摊到左右两边
    speed_l += (left_direction * msg.angular.z * base_width/2) / 2;
    speed_r -= (right_direction * msg.angular.z * base_width/2) / 2;
    ROS_INFO("Set speed left: %f, right: %f", speed_l, speed_r);


    //速度转化为目标脉冲数
    int left_except_pulse = round(speed_l / QPR); 
    int right_except_pulse = round(speed_r / QPR);
    
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

}//发布者函数结束