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
    for i, d in enumerate(distances[:1000]):
        inds = [len(circuits), len(circuits)]
        circuits.append(set([tuple(d[0]), tuple(d[1])]))
        for i in reversed(range(len(circuits))):
            if tuple(d[0]) in circuits[i]: inds[0] = i
            if tuple(d[1]) in circuits[i]: inds[1] = i
        if inds[0] != inds[1]:
            circuits[min(inds)].update(circuits[max(inds)])
            #if max(inds) != len(circuits) - 1:
            #    del circuits[-1]
            #del circuits[max(inds)]
            #circuits.pop(max(inds))
        #elif inds[0] != len(circuits) - 1: # both ind1 and ind2 are same value
        #    del circuits[-1]

    circuits = sorted(circuits, key = lambda d : len(d))
    lens = [len(c) for c in circuits]
    lens = sorted(list(set(lens)), reverse=True)

    print(f"8a - Production of 3 largest circuits: {math.prod(lens[0:3])}")

def b():
    junctions = [list(map(int, line.split(','))) for line in open(file_path, "r").readlines()]
    distances = [[j1, j2, math.dist(j1, j2)] for i1, j1 in enumerate(junctions) for j2 in junctions[i1 + 1:]]

    distances = sorted(distances, key = lambda d : d[2])

    connections = set()
    last_one = []
    for i, d in enumerate(distances):
        connections.update(set([tuple(d[0]), tuple(d[1])]))
        if len(connections) == len(junctions):
            last_one = d
            break

    print(f"8b - X coordinates: {last_one[0][0] * last_one[1][0]}")

a()
b()
