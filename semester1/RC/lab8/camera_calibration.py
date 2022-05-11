#!/usr/bin/env python3

import cv2
import itertools
import numpy as np
from os import listdir


def show_photo(photo):
    cv2.imshow('a', photo)
    cv2.waitKey(5000)


def get_photos(path):
    photo_names = [f for f in listdir(path) if f.endswith('.png')]
    return [cv2.imread(f'{path}/{f}') for f in photo_names]


def mark_corners(photo):
    retval, corners = cv2.findChessboardCorners(photo, (8, 5))
    if retval:
        corners = cv2.cornerSubPix(cv2.cvtColor(photo, cv2.COLOR_BGR2GRAY), corners, (11, 11), (-1, -1),
                                   (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001))
        # photo = cv2.drawChessboardCorners(photo, (8, 5), corners, retval)
        # show_photo(photo)
        return corners
    else:
        print("Couldn't find a chessboard in given photo")
        return None


def get_objpoints(width, size):
    prod = itertools.product(range(size[1]), range(size[0]))
    prod = (p[::-1] for p in prod)
    objpoints = np.array([[p1, p2, 0.0] for p1, p2 in prod], np.float32) * width
    return objpoints


def main():
    path_to_photos = './images'
    photos = get_photos(path_to_photos)
    img_shape = photos[0].shape[:2]
    img_shape = img_shape[::-1]

    images_corners = [mark_corners(photo) for photo in photos]
    imgpoints = [image_corner for image_corner in images_corners if image_corner is not None]

    objpoints = [get_objpoints(30, (8, 5))] * len(imgpoints)

    _, cameraMatrix, distCoeffs, _, _ = cv2.calibrateCamera(objpoints, imgpoints, img_shape, None, None)
    newCameraMatrix, _ = cv2.getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, img_shape, 0)
    map1, map2 = cv2.initUndistortRectifyMap(cameraMatrix, distCoeffs, None, newCameraMatrix, img_shape, 5)

    np.save('camera_calibration_data/cameraMatrix.npy', cameraMatrix)
    np.save('camera_calibration_data/distCoeffs.npy', distCoeffs)
    np.save('camera_calibration_data/map1.npy', map1)
    np.save('camera_calibration_data/map2.npy', map2)

    for photo in get_photos(path_to_photos):
        undistorted_photo = cv2.remap(photo, map1, map2, cv2.INTER_LINEAR)
        show_photo(np.hstack([photo, undistorted_photo]))


if __name__ == "__main__":
    main()
