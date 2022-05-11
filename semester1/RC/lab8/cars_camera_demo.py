#!/usr/bin/env python3

import cv2

from cars_camera import Camera, RESOLUTIONS


def main():
    cv2.namedWindow("demo")
    cam = Camera()

    while True:
        cam.keep_stream_alive()
        img = cam.get_frame()

        cv2.imshow("demo", img)
        keypress = cv2.pollKey() & 0xFF
        if keypress == ord('q'):
            break
        elif keypress == ord('+'):
            q = cam.get_quality()
            if not q == max(RESOLUTIONS.keys()):
                cam.set_quality(q + 1)
        elif keypress == ord('-'):
            q = cam.get_quality()
            if not q == min(RESOLUTIONS.keys()):
                cam.set_quality(q - 1)


if __name__ == "__main__":
    main()
