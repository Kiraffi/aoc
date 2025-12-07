import os
# Import math Library
import math
import re
from collections import defaultdict
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    split_points = set()
    map = open(file_path, "r").readlines()
    beams = set([(re.search(r"[S]", map[0]).span(0)[0], 0)])
    while len(beams) > 0:
        new_beams = set()
        for c, r in ((c, r + 1) for c, r in beams):
            if r >= len(map): continue
            if map[r][c] == '^':
                new_beams.update([(c - 1, r), (c + 1, r)])
                split_points.add((c, r))
            else:
                new_beams.add((c, r))
        beams = new_beams

    print(f"7a - Splits: {len(split_points)}")

def b():
    map = open(file_path, "r").readlines()
    split_points = set()
    beams = [defaultdict(lambda: 0) for _ in range(len(map))]
    beams[0][map[0].find('S')] = 1
    for row in range(1, len(map)):
        for col, amount in beams[row - 1].items():
            if map[row][col] == '^':
                beams[row][col - 1] += amount
                beams[row][col + 1] += amount
                split_points.add((col, row))
            else:
                beams[row][col] += amount
    print(f"7a - Splits: {len(split_points)}")
    print(f"7b - Timelines: {sum(beams[-1].values())}")

a()
b()
