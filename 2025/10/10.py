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

        def find_coeff_max_limits():
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
        find_coeff_max_limits()



        def has_only_pivot(r0):
            count = 0
            found_index = 0
            for col, coeff in enumerate(r0[0]):
                if not is_zero(coeff):
                    found_index = col
                else:
                    count +=1
            if count != len(r0[0]) - 1:
                return False
            if any(f[0] == found_index for f in frozen):
                return False
            val = (r0[1] // r0[0][found_index])
            frozen.append([found_index, val])
            return True

        def back_substitute():
            for f in frozen:
                for eq in eqs:
                    eq[1] -= f[1] * eq[0][f[0]]
                    eq[0][f[0]] = 0
            found = False
            for eq in eqs:
                found = has_only_pivot(eq) or found
            if found:
                 back_substitute()


        frozen = []
        start_time = timer()

        def get_pivot_index(r0):
            for num0 in range(len(r0[0])):
                if(not is_zero(r0[0][num0])):
                    return num0
            return len(r0[0])

        def has_multiple_row_same_pivot():
            mins = [get_pivot_index(eq) for eq in eqs ]
            for r0, m0 in enumerate(mins):
                for r1, m1 in enumerate(mins):
                    if r1 == r0:
                        continue
                    if m0 == m1 and m0 != len(eqs[0][0]):
                        return True
            return False

        def remove_pivot_from_other_rows(r0, from_row_index):
            num0 = get_pivot_index(r0)
            if num0 < len(r0[0]):
                for eq2 in eqs[from_row_index:]:
                    if r0 == eq2:
                        continue
                    if not is_zero(eq2[0][num0]):
                        diff = eq2[0][num0]
                        g = gdc(abs(r0[0][num0]), abs(diff))
                        diff = diff // g
                        multi = r0[0][num0] // g
                        for rrr in range(len(eq2[0])):
                            eq2[0][rrr] = (eq2[0][rrr] * multi) - diff * r0[0][rrr]
                        eq2[1] = eq2[1] * multi - diff * r0[1]


        def gauss_elim():
            global eqs
            global frozen
            frozen_len = -1
            while frozen_len != len(frozen) or has_multiple_row_same_pivot():
                frozen_len = len(frozen)
                for row_index in range(len(eqs)):
                    eqs.sort(key=sort_lam, reverse=True)
                    use_row = eqs[row_index]
                    remove_pivot_from_other_rows(use_row, row_index)

        gauss_elim()

        def gauss_jordan():
            for eq in reversed(eqs):
                remove_pivot_from_other_rows(eq, 0)

        #print(f"before gauss_jordan\n{"\n".join(map(str, eqs))}\n")
        #find_coeff_max_limits()

        gauss_jordan()
        #find_coeff_max_limits()

        #print(f"after gauss_jordan\n{"\n".join(map(str, eqs))}\n")

        #eqs = gauss_elim(eqs)
        back_substitute()
        #print(f"after back_substitute\n{"\n".join(map(str, eqs))}\n")

        #find_coeff_max_limits()


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

                find_coeff_max_limits()

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

