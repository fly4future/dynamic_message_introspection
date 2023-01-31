// Copyright 2021 Christophe Bedard
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <yaml-cpp/yaml.h>
#include <jsoncpp/json/json.h>

#include <string>

#include "dynmsg/message_reading.hpp"
#include "dynmsg/msg_parser_json.hpp"
#include "dynmsg/typesupport.hpp"
#include "dynmsg/yaml_utils.hpp"

#include "std_msgs/msg/header.hpp"

int main()
{
  // Start with a ROS 2 message, like a std_msgs/Header
  std_msgs::msg::Header msg;
  msg.frame_id = "my_frame";
  builtin_interfaces::msg::Time stamp;
  stamp.sec = 4;
  stamp.nanosec = 20U;
  msg.stamp = stamp;

  // Convert it into a YAML representation
  RosMessage_Cpp ros_msg;
  // Note: type support info could be retrieved through other means, see dynmsg::cpp::*
  InterfaceTypeName interface{"std_msgs", "Header"};
  ros_msg.type_info = dynmsg::cpp::get_type_info(interface);
  ros_msg.data = reinterpret_cast<uint8_t *>(&msg);
  YAML::Node yaml_msg = dynmsg::cpp::message_to_yaml(ros_msg);

  // Convert the YAML representation to a string
  const std::string yaml_string = dynmsg::yaml_to_string(yaml_msg);
  // Prints normal block-style YAML:
  //   stamp:
  //     sec: 4
  //     nanosec: 20
  //   frame_id: my_frame
  printf("%s\n", yaml_string.c_str());

  // Convert YAML string to JSON string
  YAML::Emitter emitter;
  emitter << YAML::DoubleQuoted << YAML::Flow << yaml_msg;
  std::string json_string(emitter.c_str());
  printf("%s\n", json_string.c_str());

  // Convert the JSON string back to ROS2 message
  std_msgs::msg::Header msg_from_json;
  void * ros_message = reinterpret_cast<void *>(&msg_from_json);
  dynmsg::cpp::json_and_typeinfo_to_rosmsg(ros_msg.type_info, json_string, ros_message);
  // Prints:
  //   my_frame
  //   4 s, 20 ns
  printf("%s\n", msg_from_json.frame_id.c_str());
  printf("%d s, %d ns\n", msg_from_json.stamp.sec, msg_from_json.stamp.nanosec);

  return 0;
}
