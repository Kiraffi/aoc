import os
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"




def a():
    accessed_rolls = 0

    with open(file_path, "r") as file:
        lines = file.read().splitlines()
        y_len = len(lines)
        for y in range(y_len):
            x_len = len(lines[y])
            for x in range(x_len):
                rolls = 0
                for y1 in range(-1, 2):
                    for x1 in range(-1, 2):
                        if y1 == 0 and x1 == 0:
                            continue
                        xp = x + x1
                        yp = y + y1

                        if xp < 0 or xp >= x_len or yp < 0 or yp >= y_len:
                            continue
                        if lines[yp][xp] == '@':
                            rolls += 1
                if lines[y][x] == '@' and rolls < 4:
                    print(f"{x}, {y} = {rolls}")
                    accessed_rolls += 1
        print(f"4a - Forklift can access {accessed_rolls} rolls")


def b():
    total_accessed_rolls = 0
    map = []
    with open(file_path, "r") as file:
        for line in file:
            map.append(list(line))
    accessed_new_rolls = True
    while accessed_new_rolls:
        accessed_new_rolls = False
        for (y, x) in [(y, x) for y in range(len(map)) for x in range(len(map[y]))]:
            rolls = 0
            for yp, xp in [(yp + y, xp + x) for yp in range(-1, 2) for xp in range(-1, 2)]:
                if xp < 0 or xp >= len(map[y]) or yp < 0 or yp >= len(map):
                    continue
                if map[yp][xp] == '@':
                    rolls += 1
            if map[y][x] == '@' and rolls < 5: #less than 4 adjacent + itself
                map[y][x] = '.'
                total_accessed_rolls += 1
                accessed_new_rolls = True
    print(f"4b - Forklift can remove {total_accessed_rolls} rolls")

#a()
b()
