cmake_minimum_required(VERSION 3.14)
project(autoware_interface_admission)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(autoware_common_msgs_poc REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include>")
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
ament_target_dependencies(${PROJECT_NAME} INTERFACE rclcpp autoware_common_msgs_poc)
target_link_libraries(${PROJECT_NAME} INTERFACE nlohmann_json::nlohmann_json)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} EXPORT export_${PROJECT_NAME})
ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_dependencies(rclcpp autoware_common_msgs_poc)
ament_export_dependencies(nlohmann_json)

add_executable(admission_node src/admission_node.cpp)
target_link_libraries(admission_node ${PROJECT_NAME})
install(TARGETS admission_node DESTINATION lib/${PROJECT_NAME})

add_executable(manifest_admit src/manifest_admit.cpp)
target_link_libraries(manifest_admit ${PROJECT_NAME})
install(TARGETS manifest_admit DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_admission_rule test/test_admission_rule.cpp)
  target_link_libraries(test_admission_rule ${PROJECT_NAME})
  ament_add_gtest(test_manifest_io test/test_manifest_io.cpp)
  target_link_libraries(test_manifest_io ${PROJECT_NAME})
  ament_add_gtest(test_manifest_admit test/test_manifest_admit.cpp)
  target_link_libraries(test_manifest_admit ${PROJECT_NAME})
endif()

ament_package()
