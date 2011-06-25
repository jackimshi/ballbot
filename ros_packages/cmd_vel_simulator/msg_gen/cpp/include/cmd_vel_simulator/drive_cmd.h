/* Auto-generated by genmsg_cpp for file /home/ankush/ros_tutorials/cmd_vel_simulator/msg/drive_cmd.msg */
#ifndef CMD_VEL_SIMULATOR_MESSAGE_DRIVE_CMD_H
#define CMD_VEL_SIMULATOR_MESSAGE_DRIVE_CMD_H
#include <string>
#include <vector>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/message.h"
#include "ros/time.h"


namespace cmd_vel_simulator
{
template <class ContainerAllocator>
struct drive_cmd_ : public ros::Message
{
  typedef drive_cmd_<ContainerAllocator> Type;

  drive_cmd_()
  : drive_speed(0)
  , steer_angle(0)
  {
  }

  drive_cmd_(const ContainerAllocator& _alloc)
  : drive_speed(0)
  , steer_angle(0)
  {
  }

  typedef int16_t _drive_speed_type;
  int16_t drive_speed;

  typedef int16_t _steer_angle_type;
  int16_t steer_angle;


private:
  static const char* __s_getDataType_() { return "cmd_vel_simulator/drive_cmd"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "20d51325e4a2e2c39f4c2497c2070be5"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "int16 drive_speed\n\
int16 steer_angle\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, drive_speed);
    ros::serialization::serialize(stream, steer_angle);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, drive_speed);
    ros::serialization::deserialize(stream, steer_angle);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(drive_speed);
    size += ros::serialization::serializationLength(steer_angle);
    return size;
  }

  typedef boost::shared_ptr< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator>  const> ConstPtr;
}; // struct drive_cmd
typedef  ::cmd_vel_simulator::drive_cmd_<std::allocator<void> > drive_cmd;

typedef boost::shared_ptr< ::cmd_vel_simulator::drive_cmd> drive_cmdPtr;
typedef boost::shared_ptr< ::cmd_vel_simulator::drive_cmd const> drive_cmdConstPtr;


template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const  ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> & v)
{
  ros::message_operations::Printer< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> >::stream(s, "", v);
  return s;}

} // namespace cmd_vel_simulator

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator>
struct MD5Sum< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> > {
  static const char* value() 
  {
    return "20d51325e4a2e2c39f4c2497c2070be5";
  }

  static const char* value(const  ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x20d51325e4a2e2c3ULL;
  static const uint64_t static_value2 = 0x9f4c2497c2070be5ULL;
};

template<class ContainerAllocator>
struct DataType< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> > {
  static const char* value() 
  {
    return "cmd_vel_simulator/drive_cmd";
  }

  static const char* value(const  ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> > {
  static const char* value() 
  {
    return "int16 drive_speed\n\
int16 steer_angle\n\
";
  }

  static const char* value(const  ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct IsFixedSize< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.drive_speed);
    stream.next(m.steer_angle);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct drive_cmd_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const  ::cmd_vel_simulator::drive_cmd_<ContainerAllocator> & v) 
  {
    s << indent << "drive_speed: ";
    Printer<int16_t>::stream(s, indent + "  ", v.drive_speed);
    s << indent << "steer_angle: ";
    Printer<int16_t>::stream(s, indent + "  ", v.steer_angle);
  }
};


} // namespace message_operations
} // namespace ros

#endif // CMD_VEL_SIMULATOR_MESSAGE_DRIVE_CMD_H

