import os
# Import math Library
import math
import re
from collections import defaultdict
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    lines = [l.strip() for l in open(file_path, "r").readlines()]
    shapes = []
    for ind in range(6):
        shapes.append([])
        for k in range(3):
            shapes[-1].append(lines[k + 1 + ind * 5].strip())

    #for s in shapes:
    #    print(f"{s}")

    sizes = [sum(x == '#' for l in s for x in l) for s in shapes]
    #print(f"{sizes}")

    fit_count = 0

    for l in lines[6 * 5:]:
        area_text, packet_text = l.split(':')
        packets = list(map(int, packet_text.strip().split(' ')))
        area = list(map(int, area_text.split('x')))
        #print(f"{area} - {packets}")
        area_count = area[0] * area[1]
        req_area_count = 0
        for i, p in enumerate(packets):
            req_area_count += sizes[i] * p
        if req_area_count <= area_count:
            fit_count += 1
    print(f"12a - Fits: {fit_count}")

a()
