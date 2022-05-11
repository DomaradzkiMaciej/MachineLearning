#!/usr/bin/env python3

import numpy as np
import math
import pybullet as p
import pybullet_data
import random
import time

p.connect(p.GUI)
p.setAdditionalSearchPath(pybullet_data.getDataPath())


def make_target(pos):
    ret = p.loadURDF("sphere_small.urdf", pos, globalScaling=2)
    p.changeVisualShape(ret, -1, rgbaColor=[1, 0.5, 0.7, 1])
    return ret


def rpy_to_dir(rpy):
    _, pitch, yaw = rpy
    x = math.cos(yaw) * math.cos(pitch)
    y = math.sin(yaw) * math.cos(pitch)
    z = -math.sin(pitch)

    v = np.array([x, y, z])
    norm = np.linalg.norm(v)
    return v / norm


def check_task_impl(dart_pose, target_pos, steps=None):
    if steps is None:
        steps = 500

    p.resetSimulation()
    p.loadURDF("plane.urdf")
    dart = p.loadURDF("dart.urdf")
    cube = make_target(target_pos)

    p.resetBasePositionAndOrientation(dart, dart_pose[0], dart_pose[1])
    p.resetBaseVelocity(dart, 1 * rpy_to_dir(p.getEulerFromQuaternion(dart_pose[1])))

    p.stepSimulation()
    if steps > 0:
        for i in range(steps):
            p.stepSimulation()
            time.sleep(0.01)
    else:
        while True:
            continue


def check_task0(yaw, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler([0, 0, yaw]))
    target_pos = [0, 1, 1]
    check_task_impl(dart_pose, target_pos, steps)


def check_task1(yaw, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler([0, 0, yaw]))
    target_pos = [1, 1, 1]
    check_task_impl(dart_pose, target_pos, steps)


def check_task2(pitch, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler([0, pitch, 0]))
    target_pos = [1, 0, 2]
    check_task_impl(dart_pose, target_pos, steps)


def check_task3(rpy, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler(rpy))
    target_pos = [1, -0.5, 2]
    check_task_impl(dart_pose, target_pos, steps)


def check_task4(rpy, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler(rpy))
    target_pos = [-1, -1, 0.2]
    check_task_impl(dart_pose, target_pos, steps)


def check_task5(rpy, steps=None):
    dart_pose = ([1, 0, 1], p.getQuaternionFromEuler(rpy))
    target_pos = [0, 0, 1]
    check_task_impl(dart_pose, target_pos, steps)


def check_task6(rpy, steps=None):
    dart_pose = ([-1, 1, 1], p.getQuaternionFromEuler(rpy))
    target_pos = [0, 0, 2]
    check_task_impl(dart_pose, target_pos, steps)


def check_task7(pitch, x, steps=None):
    dart_pose = ([x, 1, 1], p.getQuaternionFromEuler([0, pitch, math.radians(45)]))
    target_pos = [0, 1.5, 1.5]
    check_task_impl(dart_pose, target_pos, steps)


def check_task8(yaw, z, steps=None):
    dart_pose = ([-1, 1, z], p.getQuaternionFromEuler([0, math.radians(15), yaw]))
    target_pos = [0, -1, 1.1]
    check_task_impl(dart_pose, target_pos, steps if steps is not None else 1000)


def check_task9(pitch, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler([0, pitch, 0]))
    target_pos = [1.5, 0, 1.5]
    check_task_impl(dart_pose, target_pos, steps)


def check_task10(yaw, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler([0, 0, yaw]))
    target_pos = [-0.3, -1.3, 1]
    check_task_impl(dart_pose, target_pos, steps)


def check_task11(rpy, steps=None):
    dart_pose = ([0, 0, 1], p.getQuaternionFromEuler(rpy))
    target_pos = [1.2, 1.6, 1.7]
    check_task_impl(dart_pose, target_pos, steps if steps is not None else 1000)


def check_task12(yaw, z, steps=None):
    dart_pose = ([0.3, 0.8, z], p.getQuaternionFromEuler([0, math.radians(33), yaw]))
    target_pos = [0.5, -2, 1.8]
    check_task_impl(dart_pose, target_pos, steps if steps is not None else 1000)


def check_task13(x, z, steps=None):
    dart_pose = ([x, -1, z], p.getQuaternionFromEuler([0, math.radians(-15), math.radians(125)]))
    target_pos = [-0.4, 0.5, 1.2]
    check_task_impl(dart_pose, target_pos, steps)


# --- EDIT ONLY BELOW THIS LINE ---

# Demo, yaw 90 degrees is the solution, steps argument is optional
# check_task0(math.radians(90), steps=500)
#
# First batch of tasks, try to solve them by hand

check_task1(math.radians(45))
check_task2(math.radians(-45))

check_task3([math.radians(angle) for angle in [0,-42,-27]])
check_task4([math.radians(angle) for angle in [0,30,225]])

check_task5([math.radians(angle) for angle in [0,0,180]])
check_task6([math.radians(angle) for angle in [0,-35,-45]], 1000)

check_task7(math.radians(-35), -0.5)
check_task8(math.radians(-63), 1.7)


# Second batch of tasks, do not hardcode anything, calculate required answers here, in python.

# Task9 is given as:
dart_pos = np.array([0, 0, 1])
target_pos = np.array([1.5, 0, 1.5])
diff = target_pos - dart_pos
pitch = - math.atan2(diff[2], diff[0])
check_task9(pitch)

# Task10 is given as:
dart_pos = np.array([0, 0, 1])
target_pos = np.array([-0.3, -1.3, 1])
diff = target_pos - dart_pos
yaw = math.atan2(diff[1], diff[0])
check_task10(yaw)

# Task11 is given as:
dart_pos = np.array([0, 0, 1])
target_pos = np.array([1.2, 1.6, 1.7])
diff = target_pos - dart_pos
pitch = - math.atan2(diff[2], np.linalg.norm(diff[0:2]))
yaw = math.atan2(diff[1], diff[0])
check_task11([0,pitch, yaw])

# Task12 is given as:
dart_pos = np.array([0.3, 0.8, None])
target_pos = np.array([0.5, -2, 1.8])
diff = target_pos[0:2] - dart_pos[0:2]
pitch = math.radians(33)
yaw = math.atan2(diff[1], diff[0])
z = target_pos[2] + math.tan(pitch) * np.linalg.norm(diff)
check_task12(yaw, z)

# Task13 is given as:
dart_pos = np.array([None, -1, None])
target_pos = np.array([-0.4, 0.5, 1.2])
rpy = np.array([0, math.radians(-15), math.radians(125)])

y_diff = target_pos[1] - dart_pos[1]
x_diff = y_diff / math.tan(rpy[2])

x = target_pos[0]-x_diff
z = target_pos[2] + math.tan(rpy[1]) * np.linalg.norm([x_diff, y_diff])

check_task13(x, z)
