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
    pairs = ((c0, c1) for i1, c0 in enumerate(corners) for c1 in corners[i1 + 1:])
    part_1_distance = max((rect_v2(c0, c1) for c0, c1 in pairs))
    print(f"9a - Biggest rectangle: {part_1_distance}")

def b():
    corners = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    rect_v2 = lambda v0, v1 : (abs(v1[0] - v0[0]) + 1) * (abs(v1[1] - v0[1]) + 1)
    pairs = ((c0, c1) for i1, c0 in enumerate(corners) for c1 in corners[i1 + 1:])
    orig_distances = sorted(([rect_v2(c0, c1), c0, c1] for c0, c1 in pairs), key=lambda x:x[0], reverse=True)

    outside = lambda p0, p1, e0, e1 : max(p0, p1) <= min(e0, e1) or min(p0, p1) >= max(e0, e1)
    lines = sorted(((c, corners[i - 1]) for i, c in enumerate(corners)), key=lambda x : rect_v2(x[0], x[1]), reverse=True)
    for dist, c0, c1 in orig_distances:
        if all(outside(p0[0], p1[0], c0[0], c1[0]) or outside(p0[1], p1[1], c0[1], c1[1]) for p0, p1 in lines):
            break

    print(f"9b - Biggest rectangle: {dist}")

a()
b()
