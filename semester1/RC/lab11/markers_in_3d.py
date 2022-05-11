import sys
import time
from scipy.spatial.transform import Rotation

sys.path.insert(1, './cars_package/')

from cars_package.cars import Camera, Motors, Connection, RESOLUTIONS, Direction
from lib import *


def face_marker(marker_id, cam, motors, connection, x_max=0.3):
    while True:
        connection.keep_stream_alive()
        img = take_img(cam)

        corners, ids, _, tvecs = detect_aruco_markers(img)
        ids = ids.flatten().tolist() if ids is not None else []

        show_img_and_poll_key(img)

        if not ids or marker_id not in ids:
            motors.command(61, Direction.RIGHT)
        else:
            ind = ids.index(marker_id)
            x, _, _ = tvecs[ind].flatten()

            if x > x_max:
                motors.command(61, Direction.RIGHT)
            elif x < - x_max:
                motors.command(61, Direction.LEFT)
            else:
                break


def detect_marker(marker_id, cam, connection):
    while True:
        connection.keep_stream_alive()
        img = take_img(cam)
        show_img_and_poll_key(img)
        corners, ids, rvecs, tvecs = detect_aruco_markers(img)

        if ids is not None:
            break

    ids = ids.flatten().tolist() if ids is not None else []
    ind = ids.index(marker_id)

    rotation = Rotation.from_rotvec(rvecs[ind].flatten())
    _, yaw, _ = rotation.as_euler('xyz', degrees=True)
    x, y, z = tvecs[ind].flatten()

    return yaw, (x, y, z)


def rotate_perpendicularly_to_marker(yaw, motors, connection):
    for i in range(int((90 - abs(yaw)) / 6)):
        time.sleep(0.1)
        connection.keep_stream_alive()

        if yaw > 0:
            motors.command(66, Direction.LEFT)
        else:
            motors.command(66, Direction.RIGHT)


def turn_90_degrees(direction, motors, connection):
    for i in range(8):
        time.sleep(0.2)
        connection.keep_stream_alive()
        motors.command(66, direction)


def ride_perpendicularly_to_marker(yaw, z, motors, connection):
    for i in range(int(abs(z * np.cos((90 - yaw) / 180 * np.pi)) * 44)):
        time.sleep(0.1)
        connection.keep_stream_alive()

        motors.command(66, Direction.FORWARD)
        if i % 8 == 0:
            time.sleep(0.2)
            motors.command(61, Direction.LEFT)


def ride_forward_to_marker(marker_id, cam, motors, connection):
    face_marker(marker_id, cam, motors, connection, 0.05)
    _, (_, _, z) = detect_marker(marker_id, cam, connection)

    while z > 0.8:
        time.sleep(0.1)
        face_marker(marker_id, cam, motors, connection, 0.05)
        motors.command(66, Direction.FORWARD)
        _, (_, _, z) = detect_marker(marker_id, cam, connection)

    time.sleep(0.5)


def ride_to_marker(marker_id, cam, motors, connection):
    cam.flash_on()

    face_marker(marker_id, cam, motors, connection)
    yaw, (x, y, z) = detect_marker(marker_id, cam, connection)
    rotate_perpendicularly_to_marker(yaw, motors, connection)
    ride_perpendicularly_to_marker(yaw, z, motors, connection)

    if yaw > 0:
        turn_90_degrees(Direction.RIGHT, motors, connection)
    else:
        turn_90_degrees(Direction.LEFT, motors, connection)

    ride_forward_to_marker(marker_id, cam, motors, connection)

    cam.flash_off()


def main():
    connection = Connection()
    cam = Camera(connection=connection)
    motors = Motors(connection=connection)

    markers_to_visit = [2, 4, 6, 8]

    for marker_id in markers_to_visit:
        ride_to_marker(marker_id, cam, motors, connection)


if __name__ == "__main__":
    main()
