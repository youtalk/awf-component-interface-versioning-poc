cmake_minimum_required(VERSION 3.14)
project(autoware_component_interface_utils_poc)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(autoware_component_interface_specs_poc REQUIRED)
find_package(autoware_common_msgs_poc REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
ament_target_dependencies(${PROJECT_NAME} INTERFACE
  rclcpp autoware_component_interface_specs_poc autoware_common_msgs_poc)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp autoware_component_interface_specs_poc autoware_common_msgs_poc)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_accumulate test/test_accumulate.cpp)
  target_link_libraries(test_accumulate ${PROJECT_NAME})
  ament_add_gtest(test_broadcast test/test_broadcast.cpp)
  target_link_libraries(test_broadcast ${PROJECT_NAME})
endif()

ament_package()
