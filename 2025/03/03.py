import os
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    sum_of_highest = 0

    with open(file_path, "r") as file:
        for line in file:
            l = line.strip();
            l_len = len(l)
            largest = 0
            for i in range(l_len - 1):
                for j in range(i + 1, l_len):
                    num = 10 * int(l[i]) + int(l[j])
                    largest = max(largest, num)
            sum_of_highest += largest

        print(f"3a - Sum of highest numbers: {sum_of_highest}")


def b():
    sum_of_highest = 0

    with open(file_path, "r") as file:
        for line in file:
            l = line.strip();
            l_len = len(l)
            largest = 0
            left_index = 0
            for i in reversed(range(12)):
                single_largest = 0
                for j in range(left_index, l_len - i):
                    n = int(l[j])
                    if n > single_largest:
                        single_largest = n
                        left_index = j + 1
                largest = largest * 10 + single_largest
            sum_of_highest += largest

        print(f"3b - Sum of highest 12 numbers: {sum_of_highest}")

a()
b()
