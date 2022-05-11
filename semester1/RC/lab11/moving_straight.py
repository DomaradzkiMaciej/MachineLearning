import sys
import time

sys.path.insert(1, './cars_package/')

from cars_package.cars import Motors, Connection, Direction


def main():
    connection = Connection()
    motors = Motors(connection=connection)

    for i in range(50):
        motors.command(80, Direction.FORWARD)
        time.sleep(0.1)

    time.sleep(1)

    for i in range(50):
        motors.command(80, Direction.BACKWARD)
        time.sleep(0.1)


if __name__ == "__main__":
    main()
