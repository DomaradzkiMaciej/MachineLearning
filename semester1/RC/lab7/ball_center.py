#!/usr/bin/env python3

import cv2
import numpy as np

image = cv2.imread('ball.png')

# image crop
image = image[0:400, :, :]
cv2.imshow('a', image)
cv2.waitKey()

# conversion to HSV colorspace
hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

# positive red hue margin
lower1 = np.array([0, 100, 50])
upper1 = np.array([10, 255, 255])
mask1 = cv2.inRange(hsv_image, lower1, upper1)

# negative red hue margin
lower2 = np.array([160,100,50])
upper2 = np.array([179,255,255])
mask2 = cv2.inRange(hsv_image, lower2, upper2)

mask = mask1 + mask2

cv2.imshow('a', mask)
cv2.waitKey()

# trick for finding two centers
print("center x", np.argmax(np.sum(mask, axis=0)))
print("center y", np.argmax(np.sum(mask, axis=1)))

cv2.destroyAllWindows()
