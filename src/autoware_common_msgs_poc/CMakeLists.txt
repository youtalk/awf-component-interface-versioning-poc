cmake_minimum_required(VERSION 3.14)
project(autoware_common_msgs_poc)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/ProvidedInterface.msg"
  "msg/RequiredInterface.msg"
  "msg/InterfaceManifest.msg"
  "msg/AdmissionResult.msg"
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_messages test/test_messages.cpp)
  rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
  target_link_libraries(test_messages "${cpp_typesupport_target}")
endif()

ament_package()
