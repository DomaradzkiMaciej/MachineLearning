#!/usr/bin/env python3

import pybullet as p
import time
import pybullet_data

physicsClient = p.connect(p.GUI) #or p.DIRECT for non-graphical version
p.setAdditionalSearchPath(pybullet_data.getDataPath()) #optionally
p.setGravity(0,0,-10)
planeId = p.loadURDF("plane.urdf")
cubeStartPos = [0,0,1]
cubeStartOrientation = p.getQuaternionFromEuler([0,0,0])
boxId = p.loadURDF("r2d2.urdf", cubeStartPos, cubeStartOrientation)

for i in range (5000):
	p.stepSimulation()
	time.sleep(1./240.)
	cubePos, cubeRot = p.getBasePositionAndOrientation(boxId)
	print(f"pos: {[f'{x:.2f}' for x in cubePos]}, rot: {[f'{x:.5f}' for x in cubeRot]}")

p.disconnect()
