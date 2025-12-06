import os
# Import math Library
import math
import re
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    sum_of_numbers = 0
    numbers = []
    with open(file_path, "r") as file:
        for line in file:
            count = 0
            for i in re.findall(r"[+*]", line):
                number = 0 if i == '+' else 1
                for j in numbers[count]:
                    number = (number + j) if i == '+' else (number * j)
                sum_of_numbers += number
                count += 1
            for i in re.findall(r"\d+", line):
                if len(numbers) <= count:
                    numbers.append([])
                numbers[count].append(int(i))
                count += 1

    print(f"6a - Sum of numbers: {sum_of_numbers}")

def b():
    sum_of_numbers = 0
    lines = open(file_path, "r").readlines()
    max_len = max(list([len(l) for l in lines]))
    op_chars = [r.span(0)[0] for r in re.finditer(r"[+*]", lines[-1])]
    # adding last one for ending
    op_chars.append(max_len + 2)
    for i in range(len(op_chars) - 1):
        is_plus = lines[-1][op_chars[i]] == '+'
        outer_number = 0 if is_plus else 1
        for char_index in range(op_chars[i], op_chars[i + 1] - 1):
            number = []
            for l in lines:
                if char_index < len(l) and l[char_index].isdigit():
                    number.append(l[char_index])
            number = int("".join(number)) if len(number) > 0 else 0
            outer_number = (outer_number + number) if is_plus else (outer_number * number)
            #print(f"inner : {number}")
        #print(f"outer : {outer_number}")
        sum_of_numbers += outer_number

    print(f"6b - Sum of numbers: {sum_of_numbers}")

a()
b()
