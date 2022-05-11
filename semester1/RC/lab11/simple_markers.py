import sys
import time

sys.path.insert(1, './cars_package/')

from cars_package.cars import Camera, Motors, Connection, Direction
from lib import *


def main():
    connection = Connection()
    cam = Camera(connection=connection)
    motors = Motors(connection=connection)

    while True:
        connection.keep_stream_alive()
        img = take_img(cam)
        corners, ids, _, _ = detect_aruco_markers(img)
        show_img_and_poll_key(img)

        if ids is None:
            continue

        marker_id = ids[0].flatten()
        if marker_id == 2:
            motors.command(80, Direction.FORWARD)
        if marker_id == 4:
            motors.command(80, Direction.LEFT)
        if marker_id == 6:
            motors.command(80, Direction.RIGHT)
        if marker_id == 8:
            motors.command(80, Direction.BACKWARD)

        time.sleep(0.1)


if __name__ == "__main__":
    main()
