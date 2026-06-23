FROM ros:jazzy-ros-base

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get install -y --no-install-recommends \
      python3-colcon-common-extensions \
      python3-vcstool \
      ros-jazzy-rmw-cyclonedds-cpp \
      ros-jazzy-launch-testing-ament-cmake \
      ros-jazzy-launch-pytest \
    && rm -rf /var/lib/apt/lists/*

ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
ENV ROS_DOMAIN_ID=1

WORKDIR /ws

# Vendored message deps (verbatim structs reference real message types).
COPY autoware_msgs.repos /ws/autoware_msgs.repos
RUN mkdir -p src/deps && vcs import src/deps < autoware_msgs.repos

# Project sources.
COPY src/ /ws/src/

RUN source /opt/ros/jazzy/setup.bash \
    && apt-get update \
    && rosdep install --from-paths src --ignore-src -y --rosdistro jazzy || true \
    && rm -rf /var/lib/apt/lists/*

RUN source /opt/ros/jazzy/setup.bash \
    && colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=Release

COPY config/ /ws/config/

# Default command: build (already cached) then run all tests.
CMD source /opt/ros/jazzy/setup.bash \
    && source install/setup.bash \
    && colcon test --event-handlers console_direct+ \
    && colcon test-result --verbose
