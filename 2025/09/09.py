import os
# Import math Library
import math
import re
from collections import defaultdict
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    corners = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    rect_v2 = lambda v0, v1 : (abs(v1[0] - v0[0]) + 1) * (abs(v1[1] - v0[1]) + 1)
    part_1_distance = max([rect_v2(j1, j2) for i1, j1 in enumerate(corners) for j2 in corners[i1 + 1:]])
    print(f"9a - Biggest rectangle: {part_1_distance}")

def b():
    corners = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    rect_v2 = lambda v0, v1 : (abs(v1[0] - v0[0]) + 1) * (abs(v1[1] - v0[1]) + 1)
    min_v2 = lambda v0, v1 : [min(v0[0], v1[0]), min(v0[1], v1[1])]
    max_v2 = lambda v0, v1 : [max(v0[0], v1[0]), max(v0[1], v1[1])]

    distances = []
    for i1, j1 in enumerate(corners):
        for j2 in corners[i1 + 1:]:
            c0, c1 = [min_v2(j1, j2), max_v2(j1, j2)]
            if all( (p0[0] <= c0[0] and p1[0] <= c0[0]) or \
                    (p0[0] >= c1[0] and p1[0] >= c1[0]) or \
                    (p0[1] <= c0[1] and p1[1] <= c0[1]) or \
                    (p0[1] >= c1[1] and p1[1] >= c1[1]) \
                for p0, p1 in ((p0, corners[i - 1]) for i, p0 in enumerate(corners))):

                distances.append(rect_v2(j1, j2))

    print(f"9b - Biggest rectangle: {sorted(distances)[-1]}")

a()
b()
