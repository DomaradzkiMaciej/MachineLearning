#!/usr/bin/env python3

import math
import pybullet as p
import pybullet_data
from time import sleep
from numpy.linalg import norm


def doBasicSetup():
    # Basic setup
    p.connect(p.GUI)
    p.setAdditionalSearchPath(pybullet_data.getDataPath())
    p.loadURDF("plane.urdf", [0, 0, 0])
    p.setGravity(0, 0, -10)


def loadKuka():
    # This time we use kuka from pybullet_data
    kukaId = p.loadSDF("kuka_iiwa/kuka_with_gripper.sdf")[0]
    p.resetBasePositionAndOrientation(kukaId, [0, 0, 0], [0, 0, 0, 1])

    return kukaId


def setCubeColor(cube, color=None):
    if color is None:
        color = [1, 0.5, 0.7, 1]
    p.changeVisualShape(cube, -1, rgbaColor=color)


def loadCube(pos, size=0.05, color=None):
    print(f"Creating cube at {[(f'{x:.2f}') for x in pos]}.")
    ret = p.loadURDF("cube.urdf", pos, globalScaling=size)
    setCubeColor(ret, color)
    return ret


def createLineSegments(start, end, samples=100):
    distances = [(e - s) / samples for s, e in zip(start, end)]

    ret = [start]
    for _ in range(samples):
        ret.append([last + d for last, d in zip(ret[-1], distances)])

    return ret


def createPathSegments(points, samples=100):
    ret = []
    for i in range(len(points) - 1):
        ret += createLineSegments(points[i], points[i + 1], samples)

    return ret


cubeTraces = []
def createTrace(cube):
    global cubeTraces

    pos, _ = p.getBasePositionAndOrientation(cube)
    if pos[2] > 0.04:
        return

    for trace in cubeTraces:
        if norm([pos[0] - trace[0], pos[1] - trace[1]]) <= 0.02:
            return

    trace = loadCube(pos, color=[1, 0.4, 0.6, 0.6])
    p.changeDynamics(trace, -1, mass=0)
    p.setCollisionFilterGroupMask(trace, -1, 0, 0)
    cubeTraces.append(pos)


doBasicSetup()
kukaId = loadKuka()
cubes = [loadCube([-0.2, 0.7, 0.1]), loadCube([0.7, -0.7, 0.1]), loadCube([-0.7, -0.7, 0.1])]

# In our robot, end effector link has following index:
KUKA_TIP_IDX = 6

# We will control this number of joints:
KUKA_NUM_JOINTS = 7

Z_UP = 0.35
Z_DOWN = 0.3

positions = createPathSegments([(0, 0.7, Z_UP, 180, 0, 180),                #start position

                                (-0.22, 0.72, Z_UP, 180, 0, 180),
                                (-0.22, 0.72, Z_DOWN, 180, 0, 180),
                                (-0.18, 0.68, Z_DOWN, 180, 0, 205),
                                (-0.1, 0.4, Z_DOWN, 180, 0, 205),
                                (-0.12, 0.42, Z_DOWN, 180, 0, 205),
                                (-0.12, 0.42, Z_UP, 180, 0, 205),

                                (-0.14, 0.32, Z_UP, 180, 0, 165),
                                (-0.14, 0.32, Z_DOWN, 180, 0, 165),
                                (-0.08, 0.42, Z_DOWN, 180, 0, 165),
                                (-0.04, 0.7, Z_DOWN, 180, 0, 165),
                                (-0.06, 0.68, Z_DOWN, 180, 0, 165),
                                (-0.06, 0.68, Z_UP, 180, 0, 165),

                                (-0.04, 0.76, Z_UP, 180, 0, 205),
                                (-0.04, 0.76, Z_DOWN, 180, 0, 205),
                                (0.02, 0.72, Z_DOWN, 180, 0, 205),
                                (0.12, 0.46, Z_DOWN, 180, 0, 205),
                                (0.14, 0.47, Z_DOWN, 180, 0, 205),
                                (0.14, 0.47, Z_UP, 180, 0, 205),

                                (0.06, 0.40, Z_UP, 180, 0, 165),
                                (0.06, 0.40, Z_DOWN, 180, 0, 165),
                                (0.14, 0.4, Z_DOWN, 180, 0, 165),
                                (0.24, 0.7, Z_DOWN, 180, 0, 165),
                                (0.22, 0.68, Z_DOWN, 180, 0, 165),
                                (0.22, 0.68, Z_UP, 180, 0, 165),

                                (0.7, 0.0, Z_UP, 180, 0, 180),             # end position
                                ])

posIdx = 0
while True:
    pos = positions[posIdx][0:3]
    rpyDeg = positions[posIdx][3:6]
    rot = p.getQuaternionFromEuler([math.radians(deg) for deg in rpyDeg])

    # IK calculates joint positions from effector position and orientation
    jointPos = p.calculateInverseKinematics(kukaId,
                                            KUKA_TIP_IDX,
                                            pos,
                                            rot)

    # Take calculated joint positions and set as setpoints
    for i in range(KUKA_NUM_JOINTS):
        p.setJointMotorControl2(bodyIndex=kukaId,
                                jointIndex=i,
                                controlMode=p.POSITION_CONTROL,
                                targetPosition=jointPos[i],
                                targetVelocity=0,
                                force=500)

    for i in range(KUKA_NUM_JOINTS, 14):
        p.setJointMotorControl2(bodyIndex=kukaId,
                                jointIndex=i,
                                controlMode=p.POSITION_CONTROL,
                                targetPosition=0,
                                targetVelocity=0,
                                force=500)

    p.stepSimulation()
    createTrace(cubes[0])
    sleep(0.02)

    if posIdx < len(positions) - 1:
        posIdx += 1
