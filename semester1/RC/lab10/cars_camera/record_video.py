#!/usr/bin/env python3

import cv2.cv2 as cv2
import numpy as np

from cars_camera import Camera, RESOLUTIONS


def main():
    cv2.namedWindow("demo")
    cam = Camera()

    map1 = np.load('../camera_calibration_data/map1.npy')
    map2 = np.load('../camera_calibration_data/map2.npy')

    frames = []
    recording_started = False
    while True:
        cam.keep_stream_alive()
        frame = cam.get_frame()
        frame = cv2.remap(frame, map1, map2, cv2.INTER_LINEAR)

        cv2.imshow("demo", frame)
        keypress = cv2.pollKey() & 0xFF
        if keypress == ord(' '):
            recording_started = True
        elif keypress == ord('q'):
            break

        if recording_started:
            frames.append(frame)

    height, width, _ = frames[0].shape
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video = cv2.VideoWriter('../videos/video3.mp4', fourcc, 20, (width, height))

    for frame in frames:
        video.write(frame)

    cv2.destroyAllWindows()
    video.release()


if __name__ == "__main__":
    main()
