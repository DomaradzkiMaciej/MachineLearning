#!/usr/bin/env python3

import time

from cars import Camera, Motors, Connection, RESOLUTIONS, Direction


def main():
    connection = Connection()
    cam = Camera(connection=connection)
    motors = Motors(connection=connection)

    cam.flash_on()
    time.sleep(0.1)
    cam.flash_off()
    time.sleep(0.1)

    while True:
        for pos in range(1030,1501,10):
            motors.command_servo(pos)
            time.sleep(0.05)
        for pos in reversed(range(1030,1501,10)):
            motors.command_servo(pos)
            time.sleep(0.05)


if __name__ == "__main__":
    main()
