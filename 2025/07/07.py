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
    beams = defaultdict(lambda: 0)
    beams[(re.search(r"[S]", map[0]).span(0)[0], 0)] = 1
    splits = {}
    while len(beams) > 0:
        new_beams = defaultdict(lambda: 0)
        for key, value in beams.items():
            c, r = key[0], key[1] + 1
            if r >= len(map): continue
            if map[r][c] == '^':
                new_beams[(c - 1, r)] = new_beams[(c - 1, r)] + value
                new_beams[(c + 1, r)] = new_beams[(c + 1, r)] + value
            else:
                new_beams[(c, r)] = new_beams[(c, r)] + value
        split_count = 0
        for s in beams.values():
            split_count += s
        beams = new_beams
    print(f"7b - Splits: {split_count}")

a()
b()
