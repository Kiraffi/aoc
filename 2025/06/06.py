import os
# Import math Library
import math
import re
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "test.txt"

def a():
    sum_of_numbers = 0
    lines = open(file_path, "r").readlines()
    opers = re.findall(r"[+*]", lines[-1])
    numbers = [re.findall(r"\d+", l) for l in lines[0: -1]]
    for i in range(len(opers)):
        ns = [int(nums[i]) for nums in numbers]
        sum_of_numbers += sum(ns) if opers[i] == '+' else math.prod(ns)

    print(f"6a - Sum of numbers: {sum_of_numbers}")

def b():
    sum_of_numbers = 0
    lines = open(file_path, "r").readlines()
    max_len = max(list([len(l) for l in lines]))

    numbers = []
    for char_index in reversed(range(max_len)):
        number = ""
        for l in lines[:-1]:
            c = l[char_index] if char_index < len(l) else ' '
            number = number + c.strip()
        if len(number) > 0: numbers.append(int(number))
        c = lines[-1][char_index] if char_index < len(lines[-1]) else ' '
        if c == '+' or c == '*':
            sum_of_numbers += sum(numbers) if c == '+' else math.prod(numbers)
            numbers.clear()

    print(f"6b - Sum of numbers: {sum_of_numbers}")

a()
b()
