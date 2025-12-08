import os
# Import math Library
import math
import re
from collections import defaultdict
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    junctions = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    distances = [[j1, j2, math.dist(j1, j2)] for i1, j1 in enumerate(junctions) for j2 in junctions[i1 + 1:]]
    distances = sorted(distances, key = lambda d : d[2])

    circuits = []
    lam = lambda x: min([i if x in c else len(circuits) - 1 for i, c in enumerate(circuits)])
    for d in distances[:1000]:
        circuits.append(set([tuple(d[0]), tuple(d[1])]))
        inds = [lam(tuple(dis)) for dis in d[0:2]]
        if inds[0] != inds[1]:
            circuits[min(inds)].update(circuits[max(inds)])

    lens = sorted(list(set([len(c) for c in circuits])))

    print(f"8a - Production of 3 largest circuits: {math.prod(lens[-3:])}")

def b():
    junctions = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    distances = [[j1, j2, math.dist(j1, j2)] for i1, j1 in enumerate(junctions) for j2 in junctions[i1 + 1:]]
    distances = sorted(distances, key = lambda d : d[2])

    connections = set()
    for d in distances:
        connections.update(set([tuple(d[0]), tuple(d[1])]))
        if len(connections) == len(junctions):
            break

    print(f"8b - X coordinates: {d[0][0] * d[1][0]}")

a()
b()
