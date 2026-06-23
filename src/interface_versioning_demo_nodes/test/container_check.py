#!/usr/bin/env python3
"""Subscribe to /system/admission_result and assert the verdict, then exit."""
import os
import sys
import time

import rclpy
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy

from autoware_common_msgs_poc.msg import AdmissionResult

IF_NAME = "/perception/object_recognition/objects"
EXPECT_ACCEPTED = os.environ.get("EXPECT_ACCEPTED", "true").lower() == "true"
TIMEOUT_S = float(os.environ.get("CHECK_TIMEOUT", "40"))
LATCHED = QoSProfile(
    depth=10,
    reliability=ReliabilityPolicy.RELIABLE,
    durability=DurabilityPolicy.TRANSIENT_LOCAL,
)


def main():
    rclpy.init()
    node = rclpy.create_node("container_check")
    received = []
    node.create_subscription(
        AdmissionResult, "/system/admission_result", lambda m: received.append(m), LATCHED
    )
    deadline = time.time() + TIMEOUT_S
    verdict = None
    while time.time() < deadline:
        rclpy.spin_once(node, timeout_sec=0.5)
        matches = [r for r in received if r.interface_name == IF_NAME]
        if matches:
            verdict = matches[-1]
            break
    node.destroy_node()
    rclpy.shutdown()

    if verdict is None:
        print("FAIL: no AdmissionResult received across containers", flush=True)
        sys.exit(2)
    print(
        f"verdict: accepted={verdict.accepted} error_code={verdict.error_code} "
        f"reason='{verdict.reason}'",
        flush=True,
    )
    if verdict.accepted is EXPECT_ACCEPTED:
        print("PASS", flush=True)
        sys.exit(0)
    print(f"FAIL: expected accepted={EXPECT_ACCEPTED}", flush=True)
    sys.exit(1)


if __name__ == "__main__":
    main()
