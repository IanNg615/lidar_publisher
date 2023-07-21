#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "lidar_publisher_messages/msg/lidarpolarpoints.hpp"
#include <iostream>

#define be RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT

class sick_reader : public rclcpp::Node
{
public:
  sick_reader()
  :  Node("sickreader_reader"),
     best_effort(rclcpp::KeepLast(10))
     {
       auto sick_scanner_callback= [this](const sensor_msgs::msg::LaserScan::SharedPtr msg){
         auto rangesmsg = lidar_publisher_messages::msg::Lidarpolarpoints();
         auto ranges_recieved = msg->ranges;
         double angle_inc = msg -> angle_increment;
         double angle_ray = msg -> angle_min;
         //The cout below will output the size of array used for type def in labview 
         //std::cout<<"size of array is: "<<ranges_recieved.size()<<"\n";
         for(int i=0; i<(int)ranges_recieved.size(); i++)
         {
           rangesmsg.range.push_back(ranges_recieved[i]);
           rangesmsg.angle.push_back(angle_ray*180/3.14159);
           angle_ray += angle_inc;
         }
         publisher_->publish(rangesmsg);
       };
       
       subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>("scan_unfiltered", best_effort.reliability(be),sick_scanner_callback);
       
       publisher_ = this->create_publisher<lidar_publisher_messages::msg::Lidarpolarpoints>("lidarpolarpoints",10);
     }
     
     


private:
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
  rclcpp::Publisher<lidar_publisher_messages::msg::Lidarpolarpoints>::SharedPtr publisher_;
  rclcpp::QoS best_effort;


};
int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<sick_reader>());
  rclcpp::shutdown();
  return 0;
}
