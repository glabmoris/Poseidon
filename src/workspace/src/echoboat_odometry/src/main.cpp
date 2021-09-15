#include <ros/ros.h>
#include <tf2_ros/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/NavSatFix.h>
#include <sbg_driver/SbgEkfNav.h>

double vx = 0.1;
double vy = -0.1;
double vth = 0.1;


void processGpsCallback(const sensor_msgs::NavSatFix & msg){
	std::cout << "Got GPS data" << std::endl;
}

void processSBGCallback(const sbg_driver::SbgEkfNav * msg){

}

int main(int argc, char** argv){
  ros::init(argc, argv, "odometry_publisher");

  ros::NodeHandle n;
  ros::Publisher odomPublisher = n.advertise<nav_msgs::Odometry>("odom", 50);
  ros::Subscriber gpsSubscriber = n.subscribe("fix", 1000, processGpsCallback);

  tf2_ros::TransformBroadcaster odomBroadcaster;

  ros::Time current_time, last_time;
  current_time = ros::Time::now();

  last_time = ros::Time::now();

  ros::Rate r(1.0);
  while(n.ok()){

    ros::spinOnce();               // check for incoming messages
    current_time = ros::Time::now();

    //since all odometry is 6DOF we'll need a quaternion created from yaw
    //geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    //first, we'll publish the transform over tf
    //geometry_msgs::TransformStamped odom_trans;
    //odom_trans.header.stamp = current_time;
    //odom_trans.header.frame_id = "odom";
    //odom_trans.child_frame_id = "base_link";

    //odom_trans.transform.translation.x = x;
    //odom_trans.transform.translation.y = y;
    //odom_trans.transform.translation.z = 0.0;
    //odom_trans.transform.rotation = odom_quat;

    //send the transform
    //odom_broadcaster.sendTransform(odom_trans);

    //next, we'll publish the odometry message over ROS
    //nav_msgs::Odometry odom;
    //odom.header.stamp = current_time;
    //odom.header.frame_id = "odom";

    //set the position
    //odom.pose.pose.position.x = x;
    //odom.pose.pose.position.y = y;
    //odom.pose.pose.position.z = 0.0;
    //odom.pose.pose.orientation = odom_quat;

    //set the velocity
    //odom.child_frame_id = "base_link";
    //odom.twist.twist.linear.x = vx;
    //odom.twist.twist.linear.y = vy;
    //odom.twist.twist.angular.z = vth;

    //publish the message
    //odom_pub.publish(odom);

    last_time = current_time;
    r.sleep();
  }
}
