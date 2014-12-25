#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

ros::NodeHandle nh;
sensor_msgs::Range range_msg;
ros::Publisher pub_range("range_data", &range_msg);
const int analog_pin = 0;
unsigned long range_timer;
float getRange(int pin_num)
{
  int input;
  float Volt, dist;
  input = analogRead(A0); //Take ADC value
  Volt = input*0.00488; //Conver the ADC value to Volts
  if(Volt<2.8 && Volt>0.93) //Curve fitting for 4-13 cm
  {
    dist = 2.5909*Volt*Volt-14.6564*Volt+24.8075;
  }
  else if(Volt<=0.93 && Volt>0.36) //Curve fitting for 14-32 cm
  {
    dist = 48.3450*Volt*Volt-94.8984*Volt+60.8176;
  }
  else dist = 0;
  return dist;
}

void setup()
{
  nh.initNode();
  nh.advertise(pub_range);
  range_msg.radiation_type = sensor_msgs::Range::INFRARED;
  char frameid[] = "/ir_ranger";
  range_msg.header.frame_id = frameid;
  range_msg.field_of_view = 0.01;
  range_msg.min_range = 0.03;
  range_msg.max_range = 0.4;
}

void loop()
{
  // publish the range value every 50 milliseconds
  // since it takes that long for the sensor to stabilize
  if ( (millis()-range_timer) > 50)
  {
    range_msg.range = getRange(analog_pin);
    range_msg.header.stamp = nh.now();
    pub_range.publish(&range_msg);
    range_timer = millis() + 50;
  }
  nh.spinOnce();
}
