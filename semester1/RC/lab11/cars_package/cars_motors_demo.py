#!/usr/bin/env python3

import cv2
import time

from cars import Camera, Motors, Connection, RESOLUTIONS, Direction


def main():
    connection = Connection()
    cam = Camera(connection=connection)
    motors = Motors(connection=connection)

    cam.flash_on()
    time.sleep(0.1)
    cam.flash_off()

    # Note that the car follows the desired motion only for a fraction of a second,
    # if you want to maintain the direction you need to resend the given motor command.
    # 10Hz guarantees constant motion.
    motors.command_motors(80, 0)
    time.sleep(1)
    motors.command_motors(0, 80)
    time.sleep(1)
    motors.command(80, Direction.FORWARD)
    time.sleep(1)
    motors.command(80, Direction.LEFT)
    time.sleep(1)
    motors.command(80, Direction.RIGHT)
    time.sleep(1)
    motors.command(80, Direction.BACKWARD)
    time.sleep(1)


if __name__ == "__main__":
    main()
