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

    with open(file_path, "r") as file:
        for line in file:
            l = line.strip();

            dir = -1 if l[0] == 'L' else 1
            amount = int(l[1:])
            added_number = dir * amount
            if added_number > 0:
                current_number += added_number
                while current_number >= 100:
                    current_number -= 100
                    counter += 1
            if added_number < 0:
                # add 100 if starting from zero, so we dont instantly
                # add one to counter when rotating left.
                if current_number == 0:
                    current_number = 100
                current_number += added_number
                while current_number <= 0:
                    current_number += 100
                    counter += 1
                # if stop at 0, current_number is at 100...
                current_number %= 100

        print(f"1b - Number of zeroes: {counter}")


a()
b()
