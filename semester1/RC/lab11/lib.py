import sys
import numpy as np
import cv2

map1 = np.load('camera_calibration_data/map1.npy')
map2 = np.load('camera_calibration_data/map2.npy')
camera_matrix = np.load('camera_calibration_data/cameraMatrix.npy')
dist_coeffs = np.load('camera_calibration_data/distCoeffs.npy')


def show_img_and_poll_key(img):
    cv2.imshow("Robot camera", img)
    keypress = cv2.pollKey() & 0xFF
    if keypress == ord('q'):
        sys.exit()
    else:
        return keypress


def take_img(cam):
    img = cam.get_frame()
    return cv2.remap(img, map1, map2, cv2.INTER_LINEAR)


dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_APRILTAG_16h5)
MARKER_SIDE = 0.168


def detect_aruco_markers(img):
    corners, ids, _ = cv2.aruco.detectMarkers(img, dictionary)
    cv2.aruco.drawDetectedMarkers(img, corners)
    rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(corners, MARKER_SIDE, camera_matrix, dist_coeffs)

    if rvecs is not None:
        for rvec, tvec in zip(rvecs, tvecs):
            cv2.aruco.drawAxis(img, camera_matrix, dist_coeffs, rvec, tvec, 0.1)

    return corners, ids, rvecs, tvecs
