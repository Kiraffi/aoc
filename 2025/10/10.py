import os
import math
import re
import copy
import functools
from collections import defaultdict

from timeit import default_timer as timer
from datetime import timedelta

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def a():
    button_presses_total = 0
    for l in [line.strip().split(' ') for line in open(file_path, "r").readlines()]:
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        result = [c == '#' for c in l[0][1:-1]]
        button_count = len(l[0]) - 2
        sequence_count = len(l) - 2
        for i in range(2**sequence_count):
            state = [False] * button_count
            for j in range(sequence_count):
                if (i & (2**j)):
                    for b in buttons[j]:
                        state[b] = not state[b]
            if state == result:
                presses = min(presses, int.bit_count(i))
        button_presses_total += presses
    print(f"10a - Button presses: {button_presses_total}")



eqs = []
frozen = []
limits = []
presses = 2**60

@functools.cache
def gdc(a, b):
    if a == 0 or b == 0: return 1
    v = max(a, b) % min(a, b)
    if v == 0: return min(a, b)
    return gdc(min(a, b), v)

def b():
    global eqs
    global frozen
    global limits
    global presses
    button_presses_total = 0

    total_start_time = timer()

    is_zero = lambda x : x == 0 #abs(x) < 1e-5
    is_equal = lambda x, y : abs(y - x) < 1e-5
    for line_num, l in enumerate([line.strip().split(' ') for line in open(file_path, "r").readlines()]):
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        jolts = list(map(int, l[-1][1:-1].split(',')))



        #if line_num != 159:
        #    continue
        eqs = [[[0] * len(buttons), 0, 0] for _ in range(len(jolts))]
        for count, b in enumerate(buttons):
            for j in b:
                eqs[j][0][count] = 1
        for j_ind, jolt in enumerate(jolts):
             eqs[j_ind][1] = jolt
             eqs[j_ind][2] = j_ind

        sort_lam = lambda x : sum((1 if not is_zero(y) else 0)*(2**(len(x) - i)) for i, y in enumerate(x[0]))

        eqs = sorted(eqs, key=sort_lam, reverse=True)

        variable_count = len(eqs[0][0])

        def find_limits():
            while True:
                limits_copy = copy.deepcopy(limits)
                def set_limit(eq):
                    for i in range(len(limits)):
                        if not is_zero(eq[0][i]):
                            lim_num = min(limits[i], int(math.ceil(eq[1] / eq[0][i])))
                            if lim_num > 0:
                                limits[i] = lim_num

                for eq in eqs:
                    pos_neg_coeffs = ([x > 0 and not is_zero(x), x < 0 and not is_zero(x)] for x in eq[0])
                    pos_coeffs, neg_coeffs = zip(*pos_neg_coeffs)
                    has_pos_coeffs = any(pos_coeffs)
                    has_neg_coeffs = any(neg_coeffs)

                    if not has_pos_coeffs and not has_neg_coeffs: #all zero
                        continue

                    if has_pos_coeffs and has_neg_coeffs:
                        pass
                        tmp_eq = copy.deepcopy(eq)
                        for i in range(len(limits)):
                            if not is_zero(eq[0][i]) and eq[0][i] > 0:
                                tmp_eq[1] -= eq[0][i] * limits[i]
                                tmp_eq[0][i] = 0
                        set_limit(tmp_eq)
                    else:
                        set_limit(eq)
                if all(x == y for x, y in zip(limits, limits_copy)):
                    break
        limits = [10000000000000] * len(eqs[0][0])
        find_limits()



        def find_one(item):
            one_pos = lambda x: sum(1 if is_zero(y) else 0 for y in x) == len(x) - 1
            if one_pos(item[0]):
                for zz, z in enumerate(item[0]):
                    if not is_zero(z):
                        exists = any(f[0] == zz for f in frozen)
                        if exists:
                             return False
                        val = math.floor((item[1] / z) + 0.01)
                        frozen.append([zz, val])
                return True
            return False

        def back_substitute():
            for f in frozen:
                for rr in eqs:
                    rr[1] -= f[1] * rr[0][f[0]]
                    rr[0][f[0]] = 0
            found = False
            for eq in eqs:
                found = find_one(eq) or found
            if found:
                 back_substitute()


        frozen = []
        start_time = timer()

        def has_sames():
            mins = [next((i for i, q in enumerate(eq[0]) if not is_zero(q)), len(eqs)) for eq in eqs ]
            for r0, m0 in enumerate(mins):
                for r1, m1 in enumerate(mins):
                    if r1 == r0:
                        continue
                    if m0 == m1 and m0 != len(eqs):
                        return True
            return False

        def gauss_elim():
            global eqs
            global frozen
            frozen_len = -1
            while frozen_len != len(frozen) or has_sames():
                frozen_len = len(frozen)
                for row in range(len(eqs)):
                    eqs.sort(key=sort_lam, reverse=True)
                    use_row = eqs[row]
                    min_pos_val = next((i for i, v in enumerate(use_row[0]) if not is_zero(v)), variable_count) # min(k if item[row] > 0 else len(eqs) for k, item in enumerate(eqs))
                    if min_pos_val >= variable_count:
                        continue
                    mult = use_row[0][min_pos_val]
                    for k, item in enumerate(eqs):
                        if k <= row:
                            continue
                        diff = item[0][min_pos_val] #/ mult

                        if is_zero(diff):
                            continue
                        g = gdc(abs(mult), abs(diff))
                        diff = diff // g
                        multi = mult // g
                        item[0] = [(item[0][l] * multi - use_row[0][l] * diff) for l in range(len(item[0]))]
                        item[1] = item[1] * multi  - diff * use_row[1]

                #back_substitute()
            #return eqs

        gauss_elim()

        def gauss_jordan():
            for eq in reversed(eqs):
                for num0 in range(len(eq[0])):
                    if(not is_zero(eq[0][num0])):
                        mult = eq[0][num0]
                        for eq2 in eqs:
                            if eq == eq2:
                                continue
                            if not is_zero(eq2[0][num0]):
                                diff = eq2[0][num0] #/ mult
                                g = gdc(abs(mult), abs(diff))
                                diff = diff // g
                                multi = mult // g
                                for rrr in range(len(eq2[0])):
                                    #eq2[0][rrr] = eq2[0][rrr] - diff * eq[0][rrr]
                                    eq2[0][rrr] = (eq2[0][rrr] * multi) - diff * eq[0][rrr]
                                eq2[1] = eq2[1] * multi - diff * eq[1]
                        break

        #print(f"before gauss_jordan\n{"\n".join(map(str, eqs))}\n")
        #find_limits()

        gauss_jordan()
        #find_limits()

        #print(f"after gauss_jordan\n{"\n".join(map(str, eqs))}\n")

        #eqs = gauss_elim(eqs)
        back_substitute()
        #print(f"after back_substitute\n{"\n".join(map(str, eqs))}\n")

        find_limits()


        def rec():
            global eqs
            global frozen
            global limits
            global presses

            if any(f[1] < 0 for f in frozen):
                return
            if sum(f[1] for f in frozen) >= presses:
                return
            if len(frozen) != len(eqs[0][0]):

                copy_eqs = copy.deepcopy(eqs)
                copy_frozen = copy.deepcopy(frozen)

                find_limits()

                most_numbers = [0] * len(eqs[0][0])
                for eq in eqs:
                    for iiii, vvvv in enumerate(eq[0]):
                        most_numbers[iiii] += 1 if not is_zero(vvvv) else 0

                copy_limits = copy.deepcopy(limits)
                smallest_limits = [[i, x] for i, x in enumerate(limits)]

                most_numbers, smallest_limits = zip(*sorted(zip(most_numbers, smallest_limits), reverse=True))

                for limit_value in smallest_limits:
                    if any(f[0] == limit_value[0] for f in frozen):
                        continue
                    for limit_range in range(limit_value[1] + 1):
                        eqs = copy.deepcopy(copy_eqs)
                        frozen = copy.deepcopy(copy_frozen)
                        limits = copy.deepcopy(copy_limits)
                        frozen.append([limit_value[0], limit_range])
                        back_substitute()
                        rec()

            elif len(frozen) == len(eqs[0][0]):
                #if all(x[1] == 0 for x in eqs):

                values = [0] * len(jolts)
                for f in frozen:
                    for b in buttons[f[0]]:
                        values[b] += f[1]
                if all(x == jolts[iii] for iii, x in enumerate(values)):
                    new_sum = sum(x[1] for x in frozen)
                    presses = min(presses, new_sum)
            return


        values = [0] * len(jolts)
        for f in frozen:
            for b in buttons[f[0]]:
                values[b] += f[1]
        if all(x == jolts[iii] for iii, x in enumerate(values)):
            new_sum = sum(x[1] for x in frozen)
            presses = min(presses, new_sum)
        else:
            rec()

        print(f"{line_num + 1}: {presses}")
        button_presses_total += presses

    print(f"10b - Button presses: {button_presses_total} run time: {timer() - total_start_time}")



a()
b()

