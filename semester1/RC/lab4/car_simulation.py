#!/usr/bin/env python3

import pybullet as p
import pybullet_data
import math
import random

p.connect(p.GUI)
p.setAdditionalSearchPath(pybullet_data.getDataPath())


def make_boxes():
    pos_1 = [random.random(), random.random(), 0]
    angle = 3.1415 * 2 * random.random()
    pos_2 = [pos_1[0] + math.sin(angle) * 0.6, pos_1[1] - math.cos(angle) * 0.6, 0]
    return pos_1, pos_2


def build_world_with_car(pos):
    p.resetSimulation()
    p.setGravity(0, 0, -10)
    p.loadURDF("plane.urdf")
    car = p.loadURDF("racecar/racecar.urdf")
    p.resetBasePositionAndOrientation(car, pos[0], pos[1])
    return car


def simulate_car(car):
    inactive_wheels = [3, 5, 7]
    wheels = [2]
    for wheel in inactive_wheels:
        p.setJointMotorControl2(car, wheel, p.VELOCITY_CONTROL, targetVelocity=0, force=0)
    steering = [4, 6]
    maxForce = 10
    targetVelocity = -2
    steeringAngle = 0.174
    steps = 5000
    for wheel in wheels:
        p.setJointMotorControl2(car,
                                wheel,
                                p.VELOCITY_CONTROL,
                                targetVelocity=targetVelocity,
                                force=maxForce)

    for steer in steering:
        p.setJointMotorControl2(car, steer, p.POSITION_CONTROL, targetPosition=steeringAngle)
    for i in range(steps):
        p.stepSimulation()
    return p.getBasePositionAndOrientation(car)


start_pose = ((0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 1.0))
car = build_world_with_car(start_pose)
end_pose = simulate_car(car)
pos_1, pos_2 = make_boxes()

# calculation of calculated_pose
pose_between_boxes = [(a + b) / 2 for a, b in zip(pos_1, pos_2)]
angle = math.atan2(pos_1[1] - pos_2[1], pos_1[0] - pos_2[0])
angles_between_boxes = [0, 0, angle + math.pi / 2]
rot_between_boxes = p.getQuaternionFromEuler(angles_between_boxes)
inv_move = p.invertTransform(end_pose[0], end_pose[1])
calculated_pose = p.multiplyTransforms(pose_between_boxes, rot_between_boxes, inv_move[0], inv_move[1])

car = build_world_with_car(calculated_pose)
p.loadURDF("cube.urdf", pos_1, globalScaling=0.1)
p.loadURDF("cube.urdf", pos_2, globalScaling=0.1)
simulate_car(car)

while True:
    pass
