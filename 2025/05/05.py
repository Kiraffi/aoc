import os
# Import math Library
import math

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    fresh_ingredients = 0
    ranges = []
    with open(file_path, "r") as file:
        for line in file:
            if '-' in line:
                values = line.split('-')
                ranges.append((int(values[0]), int(values[1])))
            elif line[0] != '\n':
                value = int(line)
                for l, r in ranges:
                    if l <= value and r >= value:
                        fresh_ingredients += 1
                        break
    print(f"5a - Fresh ingredients: {fresh_ingredients}")

def b():
    ranges = []
    with open(file_path, "r") as file:
        for line in file:
            if '-' in line:
                ranges.append(list(map(int, line.strip().split('-') )))
    ranges = sorted(ranges, key = lambda num : num[0])

    fresh_ingredient_ids = 0
    curr_left = 0
    for l, r in ranges:
        l = max(l, curr_left)
        added = max(0, r - l + 1) # +1 to amount since we calculate inclusive from [a, b]
        fresh_ingredient_ids += added
        curr_left = max(curr_left, r + 1)

    print(f"5b - Fresh ingredient ids: {fresh_ingredient_ids}")

a()
b()
