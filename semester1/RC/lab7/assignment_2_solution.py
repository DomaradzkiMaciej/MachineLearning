import cv2
import numpy as np
from assignment_2_lib import take_a_photo, drive

def get_masked_image(image):
  hsv_image = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
  # positive red hue margin
  lower1 = np.array([0, 100, 50])
  upper1 = np.array([10, 255, 255])
  mask1 = cv2.inRange(hsv_image, lower1, upper1)

  # negative red hue margin
  lower2 = np.array([160, 100, 50])
  upper2 = np.array([179, 255, 255])
  mask2 = cv2.inRange(hsv_image, lower2, upper2)

  mask = mask1 + mask2
  return mask

def forward_distance(photo):
  image = photo[:470, :, :]
  mask = get_masked_image(image)
  width = np.max(np.sum(mask, axis=0))/255
  return int(750000/width-2000)

def get_ball_canter(image):
  mask = get_masked_image(image)
  center_x = np.argmax(np.sum(mask, axis=0))
  center_y = np.argmax(np.sum(mask, axis=1))
  return center_x, center_y

def turn_car_towards_ball(car):
  direction = 1
  forward = True

  while True:
    photo = take_a_photo(car)
    image = photo[:470, :, :]
    center_x, center_y = get_ball_canter(image)

    if abs(center_x-image.shape[1]/2) < 60:
      break

    drive(car, forward, direction)
    direction *= -1
    forward = not forward

def find_a_ball(car):
  turn_car_towards_ball(car)
  image = take_a_photo(car)

  for _ in range(round(forward_distance(image)/250)):
    drive(car, True, 0)

def rotate_car_90_degrees(car, direction):
  forward = True
  for _ in range(14):
    drive(car, forward, direction)
    direction *= -1
    forward = not forward

def turn_car_towards_ball2(car):
  photo = take_a_photo(car)
  image = photo[:470, :, :]
  center_x, center_y = get_ball_canter(image)

  direction = -1 if center_x > image.shape[1]/2 else 1
  ball_location = direction
  forward = True

  steps = 0
  while True:
    photo = take_a_photo(car)
    image = photo[:470, :, :]
    center_x, center_y = get_ball_canter(image)

    drive(car, forward, direction)
    direction *= -1
    forward = not forward
    steps += 1

    if abs(center_x-image.shape[1]/2) < 50:
      break

  return steps, direction, forward, ball_location

def face_ball(car, ball_location):
  drive(car, True, -ball_location)

  while True:
    photo = take_a_photo(car)
    image = photo[:470, :, :]
    center_x, center_y = get_ball_canter(image)

    if abs(center_x - image.shape[1] / 2) < 40 and ball_location * (center_x - image.shape[1] / 2) < 0:
      break

    direction = -1 if center_x > image.shape[1] / 2 else 1
    drive(car, False, direction)
    drive(car, False, 0)
    drive(car, False, 0)
    drive(car, True, direction)
    drive(car, True, 0)
    drive(car, True, 0)

def ride_to_ball(car):
  photo = take_a_photo(car)
  image = photo[:470, :, :]
  steps = round(forward_distance(image) / 250)

  for _ in range(steps):
    drive(car, True, 0)

  return steps

def push_ball(car, steps):
  while True:
    photo = take_a_photo(car)
    image = photo[:470, :, :]
    mask = get_masked_image(image)
    weidth = np.max(np.sum(mask, axis=0)) / 255

    if weidth > 395:
      break

    drive(car, True, 0)

  for _ in range(steps):
    drive(car, True, 0)

def move_a_ball(car):
  rotate_car_90_degrees(car, 1)

  for _ in range(27):
    drive(car, True, -1)
    drive(car, True, 0)

  rotate_car_90_degrees(car, -1)

  for _ in range(10):
    drive(car, False, 0)

  rotation_steps, direction, forward, ball_location = turn_car_towards_ball2(car)
  ride_steps = ride_to_ball(car)

  direction *= -1
  for _ in range(rotation_steps):
    drive(car, forward, direction)
    direction *= -1
    forward = not forward

  face_ball(car, ball_location)

  push_steps = 39 if ride_steps < 25 else 37
  push_ball(car, push_steps)