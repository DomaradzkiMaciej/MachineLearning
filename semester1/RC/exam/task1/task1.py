import pandas as pd
import numpy as np
import random

from collections import namedtuple

Point = namedtuple('Point', ['x', 'y', 'z'])


def from_stereo_to_3d(p1, p2, d, M):
    x = d * (p1[0] - M[0][2]) / (p1[0] - p2[0])
    y = d * M[0][0] * (p1[1] - M[0][2]) / (M[1][1] * (p1[0] - p2[0]))
    z = d * M[0][0] / (p1[0] - p2[0])

    return x, y, z


def check_collinearity(pos1, pos2, pos3):
    d1 = np.sqrt((pos2.x - pos3.x) ** 2 + (pos2.y - pos3.y) ** 2 + (pos2.z - pos3.z) ** 2)
    d2 = np.sqrt((pos1.x - pos2.x) ** 2 + (pos1.y - pos2.y) ** 2 + (pos1.z - pos2.z) ** 2)
    d3 = np.sqrt((pos3.x - pos1.x) ** 2 + (pos3.y - pos1.y) ** 2 + (pos3.z - pos1.z) ** 2)

    return abs(d1 + d2 - d3) < 1e-10 or abs(d2 + d3 - d1) < 1e-10 or abs(d3 + d1 - d2) < 1e-10


def dist(pos1, pos2, pos3, point):
    if check_collinearity(pos1, pos2, pos3) == True:
        raise Exception('points are colinear')

    a = pos1.y * (pos2.z - pos3.z) + pos2.y * (pos3.z - pos1.z) + pos3.y * (pos1.z - pos2.z)
    b = pos1.z * (pos2.x - pos3.x) + pos2.z * (pos3.x - pos1.x) + pos3.z * (pos1.x - pos2.x)
    c = pos1.x * (pos2.y - pos3.y) + pos2.x * (pos3.y - pos1.y) + pos3.x * (pos1.y - pos2.y)
    d = -(pos1.x * (pos2.y * pos3.z - pos3.y * pos2.z) +
          pos2.x * (pos3.y * pos1.z - pos1.y * pos3.z) +
          pos3.x * (pos1.y * pos2.z - pos2.y * pos1.z))

    return abs(a * point.x + b * point.y + c * point.z + d) / np.sqrt(a * a + b * b + c * c)


def ransac(points, n_epochs=1000, threshold=0.1, inliers_min=90):
    best_n_inliers = 0
    best_ind = []

    for _ in range(n_epochs):
        inds = random.sample(range(len(points)), k=3)
        sample = points[inds]

        sample_ind = inds

        for i, p in enumerate(points):
            if i in inds:
                continue

            d = dist(Point(*sample[0]), Point(*sample[1]), Point(*sample[2]), Point(*p))
            if d <= threshold:
                sample_ind.append(i)

        n_inliers = len(sample_ind)
        if n_inliers < inliers_min:
            continue

        if best_n_inliers < n_inliers:
            best_n_inliers = n_inliers
            best_ind = sample_ind

    return best_ind


def main():
    data = pd.read_csv('dataset.csv', sep=' ')
    data = data.to_numpy()
    M = [[1, 0, 300.5],
         [0, 1, 300.5],
         [0, 0, 1]]
    d = 200

    points_in_3d = []

    for x1, y1, x2, y2 in data:
        points_in_3d.append(from_stereo_to_3d((x1, y1), (x2, y2), d, M))

    points_in_3d = np.array(points_in_3d)
    points_at_plane = ransac(points_in_3d)

    print(len(points_at_plane))
    print(sorted(points_at_plane))


if __name__ == "__main__":
    main()
