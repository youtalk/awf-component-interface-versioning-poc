cmake_minimum_required(VERSION 3.14)
project(autoware_component_interface_specs_poc)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_srvs REQUIRED)
find_package(autoware_perception_msgs REQUIRED)
find_package(autoware_planning_msgs REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
ament_target_dependencies(${PROJECT_NAME} INTERFACE
  rclcpp std_srvs autoware_perception_msgs autoware_planning_msgs)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp std_srvs autoware_perception_msgs autoware_planning_msgs)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_version test/test_version.cpp)
  target_link_libraries(test_version ${PROJECT_NAME})

  ament_add_gtest(test_concept test/test_concept.cpp)
  target_link_libraries(test_concept ${PROJECT_NAME})
endif()

ament_package()
