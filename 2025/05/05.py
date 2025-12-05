import os
# Import math Library
import math

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    fresh_ingredients = 0
    ranges = []
    with open(file_path, "r") as file:
        parsing_ranges = True
        for line in file:
            if parsing_ranges:
                if len(line) == 1:
                    parsing_ranges = False
                else:
                    values = line.split('-')
                    ranges.append((int(values[0]), int(values[1])))
            else:
                value = int(line)
                for r in ranges:
                    if r[0] <= value and r[1] >= value:
                        fresh_ingredients += 1
                        break
    print(f"5a - Fresh ingredients: {fresh_ingredients}")

def b():
    ranges = []
    with open(file_path, "r") as file:
        parsing_ranges = True
        for line in file:
            if parsing_ranges:
                if len(line) == 1:
                    parsing_ranges = False
                else:
                    values = line.split('-')
                    ranges.append((int(values[0]), int(values[1])))
    ranges = sorted(ranges, key = lambda num : num[0])

    fresh_ingredient_ids = 0
    last_ingredient = -1
    for r in ranges:
        l = max(last_ingredient, r[0])
        # +1 since we calculate inclusive from [a, b]
        added = max(0, r[1] - l + 1)
        fresh_ingredient_ids += added
        last_ingredient = max(last_ingredient, r[1] + 1)

    print(f"5b - Fresh ingredient ids: {fresh_ingredient_ids}")

a()
b()
