#include "bamboo/BambooMotor.h"

/*
//使用ctrl+C可以退出程序
int quit_flag = 0;
void exit_handler(int sig)
{
    quit_flag = 1;
}

//记录循环次数
int loopcount = 0;
*/


int main(int argc, char *argv[])
{   
    //按键捕获，用于退出while循环
    //signal(SIGINT,exit_handler);

    ros::init(argc, argv, "arduino_due_node");
    BambooMotor bamboo_motor;
    return 0;
}
