#!/usr/bin/env python3

import pybullet as p
import pybullet_data
import time
from numpy import random

NUMBER_OF_CUBES = 4


def doBasicSetup():
    # start the simulation with a GUI (p.DIRECT is without GUI)
    p.connect(p.GUI)

    # we can load plane and cube from pybullet_data
    p.setAdditionalSearchPath(pybullet_data.getDataPath())

    # load a plane
    p.loadURDF("plane.urdf", [0, 0, -0.1], useFixedBase=True)

    # setup gravity (without it there is no gravity at all)
    p.setGravity(0, 0, -10)


def loadCubeRandomnly():
    min_pos = [-8, -8, 0]
    max_pos = [8, 8, 1]
    rand_pos = random.randint(min_pos, max_pos, 3) / 10
    cube = p.loadURDF("cube.urdf", rand_pos, globalScaling=0.05)
    p.changeVisualShape(cube, -1, rgbaColor=[0.7, 0.1, 0.7, 1])

    return cube


def loadCubes(numberOfCubes):
    cubes = [loadCubeRandomnly() for _ in range(4)]
    return cubes


def displayRobotJointsInfo(robot):
    numJoints = p.getNumJoints(robot)
    for joint in range(numJoints):
        print(p.getJointInfo(robot, joint))


def addSliders():
    sliders = [p.addUserDebugParameter("z", -0.1, 0, 0),
               p.addUserDebugParameter("y", -1, 1, 0),
               p.addUserDebugParameter("x", -1, 1, 0),
               p.addUserDebugParameter("pos", 0.0, 6.28, 0)]

    return sliders


def moveRobotAccordingsToSliders(robot, sliders):
    for idx, slider in enumerate(sliders):
        p.setJointMotorControl2(robot, idx, p.POSITION_CONTROL, p.readUserDebugParameter(slider))

    # step Simulation
    p.stepSimulation()
    changeCubesColorBasedOnPossitions(cubes)
    time.sleep(0.01)  # sometimes pybullet crashes, this line helps a lot


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


def isCubeInCentre(cube):
    pos, _ = p.getBasePositionAndOrientation(cube)
    if abs(pos[0]) <= 0.25 and abs(pos[1]) <= 0.25:
        return True
    else:
        return False


cubesColors = [[0.7, 0.1, 0.7, 1] for _ in range(NUMBER_OF_CUBES)]
def changeCubesColorBasedOnPossitions(cubes):
    global cubesColors

    for i, (cube, color) in enumerate(zip(cubes, cubesColors)):
        if isCubeInCentre(cube):
            newColor = [0.1, 0.7, 0.1, 1]
        else:
            newColor = [0.7, 0.1, 0.7, 1]

        if newColor != color:
            p.changeVisualShape(cube, -1, rgbaColor=newColor)
            cubesColors[i] = newColor


def moveCube(cube):
    Z_UP = -0.04
    Z_DOWN = -0.075

    (x, y, z), _ = p.getBasePositionAndOrientation(cube)
    x_sign = x / abs(x) if x != 0 else 1
    y_sign = y / abs(y) if y != 0 else 1
    positions = createPathSegments([(0, 0, Z_UP),
                                    (x + 0.06 * x_sign, y, Z_UP),
                                    (x + 0.06 * x_sign, y, Z_DOWN),
                                    (0.06 * x_sign, y, Z_DOWN),
                                    (0.07 * x_sign, y, Z_UP),
                                    (0, y + 0.06 * y_sign, Z_UP),
                                    (0, y + 0.06 * y_sign, Z_DOWN),
                                    (0, 0.06 * y_sign, Z_DOWN),
                                    (0, 0.07 * y_sign, Z_UP),
                                    (0, 0, Z_UP)])

    for pos in positions:
        (x, y, z) = pos
        p.setJointMotorControl2(robot, 0, p.POSITION_CONTROL, z, maxVelocity=1.5)
        p.setJointMotorControl2(robot, 1, p.POSITION_CONTROL, y, maxVelocity=1.5)
        p.setJointMotorControl2(robot, 2, p.POSITION_CONTROL, x, maxVelocity=1.5)

        p.stepSimulation()
        changeCubesColorBasedOnPossitions(cubes)
        time.sleep(0.04)  # sometimes pybullet crashes, this line helps a lot

    p.setJointMotorControl2(robot, 3, p.POSITION_CONTROL, 0)


doBasicSetup()
# load our robot definition
robot = p.loadURDF("robot.urdf")
# load cubes
cubes = loadCubes(NUMBER_OF_CUBES)
# display info about robot joints
displayRobotJointsInfo(robot)
# add four sliders to GUI
sliders = addSliders()


while True:
    areAllCubesInCenter = True
    cubeNotInCenter = None
    for cube in cubes:
        if not isCubeInCentre(cube):
            areAllCubesInCenter = False
            cubeNotInCenter = cube
            break

    if not areAllCubesInCenter:
        moveCube(cubeNotInCenter)
    else:
        moveRobotAccordingsToSliders(robot, sliders)
