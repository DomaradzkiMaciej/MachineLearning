#!/usr/bin/env python3

import cv2
from os import listdir

from cars_camera import Camera


def get_max_number_of_photos(path):
    photos_names = [f for f in listdir(path) if f.endswith('.png')]
    photos_numbers = [int(f[len('photo'): -len('.png')]) for f in photos_names]
    return max(photos_numbers, default=0)


def main():
    cv2.namedWindow("demo")
    cam = Camera()
    n_photo = get_max_number_of_photos('./images')+1

    while True:
        cam.keep_stream_alive()
        img = cam.get_frame()

        cv2.imshow("demo", img)
        keypress = cv2.pollKey() & 0xFF
        if keypress == ord('q'):
            break
        elif keypress == ord(' '):
            cv2.imwrite(f'./images/photo{n_photo}.png', img)
            print("a picture was taken")
            n_photo += 1


if __name__ == "__main__":
    main()
