import os
# Import math Library
import math
import re
from collections import defaultdict
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    lines = [list(line.split(': ')) for line in open(file_path, "r").readlines()]
    connections = {}
    for l in lines:
        connections[l[0]] = l[1].strip().split(' ')

    routes = ['you']

    while any(r != 'out' for r in routes) and len(routes) > 0:
        for i in reversed(range(len(routes))):
            if routes[i] != 'out':
                routes = [*routes, *connections[routes[i]]]
                del routes[i]


    print(f"11a - Routes: {len(routes)}")

def b():
    lines = [list(line.split(': ')) for line in open(file_path, "r").readlines()]
    connections = {}
    for l in lines:
        connections[l[0]] = l[1].strip().split(' ')

    # state = name, dfs_bit, fft_bit
    cache = {}

    def rec(state):
        sum = 0
        if state[0] == 'out':
            return 1 if state[1] and state[2] else 0
        for c in connections[state[0]]:
            fft = state[1] or c == 'fft'
            dac = state[2] or c == 'dac'
            new_state = [c, fft, dac]
            if tuple(new_state) in cache:
                sum += cache[tuple(new_state)]
            else:
                v = rec(new_state)
                cache[tuple(new_state)] = v
                sum += v
        return sum

    print(f"11b - Routes: { rec(['svr', False, False]) }")

a()
b()
