#ifndef COMMON
#define COMMON

//所以平时需要用到的头文件都放在这里
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <typeinfo>
#include <inttypes.h>

#include <cmath>
#include <cstring>
#include <signal.h>
#include "serial/serial.h"


//ros头文件
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Quaternion.h"
#include "nav_msgs/Odometry.h"
#include "tf/transform_broadcaster.h"



using namespace std;
using namespace serial;


#endif // COMMON