import sys
import time

sys.path.insert(1, './cars_package/')

from cars_package.cars import Camera, Motors, Connection, Direction
from lib import *


def main():
    connection = Connection()
    cam = Camera(connection=connection)
    motors = Motors(connection=connection)

    photo_number = 1
    cam.flash_on()

    while True:
        connection.keep_stream_alive()
        img = take_img(cam)
        key = show_img_and_poll_key(img)

        if key == ord('w'):
            motors.command(80, Direction.FORWARD)
        if key == ord('a'):
            motors.command(70, Direction.LEFT)
        if key == ord('d'):
            motors.command(70, Direction.RIGHT)
        if key == ord('s'):
            motors.command(80, Direction.BACKWARD)
        if key == ord(' '):
            cv2.imwrite(f'./photos/photo{photo_number+20}.png', img)
            print("a picture was taken")
            photo_number += 1

        time.sleep(0.1)


if __name__ == "__main__":
    main()
