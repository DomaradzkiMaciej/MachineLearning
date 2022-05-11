import numpy as np


def is_stable(M):
    w, _ = np.linalg.eigh(M)
    return np.all(w < 0)


def main():
    alfa_0 = 0
    alfa_1 = 200

    for i in range(10000):
        alfa = (alfa_0 + alfa_1) / 2
        M = [[10 - alfa, -3],
             [-3, -100]]

        if is_stable(M):
            alfa_1 = alfa
        else:
            alfa_0 = alfa

    print(f'Smallest alfa = {alfa_1}')


if __name__ == "__main__":
    main()
