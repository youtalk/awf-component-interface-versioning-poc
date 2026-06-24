import time

import launch
import launch_pytest
import pytest
import rclpy
from launch_ros.actions import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy

from autoware_common_msgs_poc.msg import AdmissionResult

IF_NAME = "/perception/object_recognition/objects"
LATCHED = QoSProfile(
    depth=10,
    reliability=ReliabilityPolicy.RELIABLE,
    durability=DurabilityPolicy.TRANSIENT_LOCAL,
)


def _description(consumer_major):
    return launch.LaunchDescription(
        [
            Node(package="interface_versioning_demo_nodes", executable="provider_node"),
            Node(
                package="interface_versioning_demo_nodes",
                executable="consumer_node",
                parameters=[{"consumer_major": consumer_major}],
            ),
            Node(package="autoware_interface_admission", executable="admission_node"),
            launch_pytest.actions.ReadyToTest(),
        ]
    )


@launch_pytest.fixture
def accept_description():
    return _description(2)


@launch_pytest.fixture
def reject_description():
    return _description(3)


def _remap_description():
    # The provider's perception topic is remapped away; the consumer is not. Same logical
    # Spec (declared_topic) and compatible MAJOR, but disjoint resolved wire topics — the
    # false-accept that logical-name-only admission would miss.
    return launch.LaunchDescription(
        [
            Node(
                package="interface_versioning_demo_nodes",
                executable="provider_node",
                remappings=[(IF_NAME, IF_NAME + "_remapped")],
            ),
            Node(
                package="interface_versioning_demo_nodes",
                executable="consumer_node",
                parameters=[{"consumer_major": 2}],
            ),
            Node(package="autoware_interface_admission", executable="admission_node"),
            launch_pytest.actions.ReadyToTest(),
        ]
    )


@launch_pytest.fixture
def remap_description():
    return _remap_description()


def _collect_result(timeout_s):
    rclpy.init()
    node = rclpy.create_node("checker")
    received = []
    node.create_subscription(
        AdmissionResult, "/system/admission_result", lambda m: received.append(m), LATCHED
    )
    deadline = time.time() + timeout_s
    result = None
    while time.time() < deadline:
        rclpy.spin_once(node, timeout_sec=0.5)
        matches = [r for r in received if r.declared_topic == IF_NAME]
        if matches:
            result = matches[-1]
            # keep spinning briefly to let the latest verdict settle
            if result is not None:
                break
    node.destroy_node()
    rclpy.shutdown()
    return result


@pytest.mark.launch(fixture=accept_description)
def test_accept_same_major():
    result = _collect_result(20)
    assert result is not None, "no AdmissionResult for the perception interface"
    assert result.code == AdmissionResult.ACCEPTED


@pytest.mark.launch(fixture=reject_description)
def test_reject_higher_major():
    result = _collect_result(20)
    assert result is not None, "no AdmissionResult for the perception interface"
    assert result.code == AdmissionResult.MAJOR_MISMATCH


@pytest.mark.launch(fixture=remap_description)
def test_remap_topic_mismatch():
    # version-compatible provider/consumer, but the remap left them on disjoint wire topics
    result = _collect_result(20)
    assert result is not None, "no AdmissionResult for the perception interface"
    assert result.code == AdmissionResult.TOPIC_MISMATCH
