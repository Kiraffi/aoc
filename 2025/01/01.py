import os
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    current_number = 50
    counter = 0

    with open(file_path, "r") as file:
        for line in file:
            l = line.strip();

            dir = -1 if l[0] == 'L' else 1
            amount = int(l[1:])

            current_number += dir * amount
            current_number %= 100

            if current_number == 0:
                counter += 1

        print(f"1a - Number of zeroes: {counter}")

def b():
    current_number = 50
    counter = 0
    prev_dir = 1
    with open(file_path, "r") as file:
        for line in file:
            l = line.strip();

            dir = -1 if l[0] == 'L' else 1
            amount = int(l[1:])

            # invert the current value when changing rotation,
            # (dir * prev_dir) == 1 if prev_dir == dir else -1
            # if prev_dir != dir: current_number = (100 - current_number) % 100
            current_number = 100 + current_number * dir * prev_dir
            current_number %= 100

            current_number += amount
            # int division
            counter += current_number // 100

            current_number %= 100
            prev_dir = dir

        print(f"1b - Number of zeroes: {counter}")


a()
b()
