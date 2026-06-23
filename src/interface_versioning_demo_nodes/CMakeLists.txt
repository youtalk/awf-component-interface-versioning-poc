cmake_minimum_required(VERSION 3.14)
project(interface_versioning_demo_nodes)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(autoware_perception_msgs REQUIRED)
find_package(autoware_component_interface_specs_poc REQUIRED)
find_package(autoware_component_interface_utils_poc REQUIRED)
find_package(autoware_common_msgs_poc REQUIRED)

add_executable(provider_node src/provider_node.cpp)
ament_target_dependencies(provider_node
  rclcpp autoware_perception_msgs
  autoware_component_interface_specs_poc autoware_component_interface_utils_poc
  autoware_common_msgs_poc)

add_executable(consumer_node src/consumer_node.cpp)
ament_target_dependencies(consumer_node
  rclcpp autoware_perception_msgs
  autoware_component_interface_specs_poc autoware_component_interface_utils_poc
  autoware_common_msgs_poc)

install(TARGETS provider_node consumer_node DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(launch_testing_ament_cmake REQUIRED)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_handshake_launch test/test_handshake_launch.py
    TIMEOUT 120)
endif()

ament_package()
