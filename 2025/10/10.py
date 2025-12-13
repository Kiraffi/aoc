import os
import math
import re
import copy
from collections import defaultdict

from timeit import default_timer as timer
from datetime import timedelta

dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"


#  7 = A +     C
#  5 =             D + E
# 12 = A + B +     D + E
#  7 = A + B +         E
#  2 = A +     C +     E

#  7 = A +     C   D
#  5 =             D + E
# 12 = A + B +     D + E
#  7 = A + B +         E
#  2 = A +     C +     E
#  D - E = 5, E = 0, D = 5?


#  7 = A + B +
#  2 = A +     C +
# min makes A = 2, C = 0, B = 5

#  2 = A +     C
#  7 = A + B +
#  7 = A + B +
#  2 = A +     C +
#  D - E = 5, E = 0, D = 5?



#  7 = A +     C   D
#  5 =             D + E
# 12 = A + B +     D + E
#  7 = A + B +         E
#  2 = A +     C +     E








#  7 = A +     C   D   E
#  5 =         C   D
# 12 = A               E
#  7 = A + B + C
#  2 =     B   C +  D  E

#  7 = A +     C    D   E
#  5 =         C    D
#  5 =        -C   -D
#  7 =     B       -D -E
#  2 =     B   C +  D  E





def a():
    button_presses_total = 0
    for l in [line.strip().split(' ') for line in open(file_path, "r").readlines()]:
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        result = [c == '#' for c in l[0][1:-1]]
        #print(f"{buttons}, {result}")
        button_count = len(l[0]) - 2
        sequence_count = len(l) - 2
        for i in range(2**sequence_count):
            state = [False] * button_count
            for j in range(sequence_count):
                if (i & (2**j)):
                    for b in buttons[j]:
                        state[b] = not state[b]
            #tst = list(filter(lambda x:x[1] == '1', ((pos, c) for pos, c in enumerate(list(bin(i))[2:]))))
            #for j, _ in tst: # range(sequence_count):
            #    for b in buttons[j]:
            #        state[b] = not state[b]
            #        #print(f"{i}, {j}")
            #print(f"{i} {state}")
            if state == result:
                presses = min(presses, int.bit_count(i))
        button_presses_total += presses
        #print(f"{l} amount: {2**(len(l[0]) - 2)}")
    print(f"10a - Button presses: {button_presses_total}")














eqs = []
frozen = []
limits = []
presses = 2**60

def b():
    global eqs
    global frozen
    global limits
    global presses
    button_presses_total = 0

    total_start_time = timer()

    for line_num, l in enumerate([line.strip().split(' ') for line in open(file_path, "r").readlines()]):
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        jolts = list(map(int, l[-1][1:-1].split(',')))



        #if line_num != 36:
        #    continue
        eqs = [[[0] * len(buttons), 0, 0] for _ in range(len(jolts))]
        for count, b in enumerate(buttons):
            for j in b:
                eqs[j][0][count] = 1
        for j_ind, jolt in enumerate(jolts):
             eqs[j_ind][1] = jolt
             eqs[j_ind][2] = j_ind
        #print(f"{eqs} before sort")

        sort_lam = lambda x : sum((1 if y != 0 else 0)*(2**(len(x) - i)) for i, y in enumerate(x[0]))
        #for eq in eqs:
        #    print(sort_lam(eq))
        eqs = sorted(eqs, key=sort_lam, reverse=True)
        #print(f"{eqs} after sort")

        #j_copy = jolts.copy()
        variable_count = len(eqs[0][0])

        ##print(f"-----")


        def find_limits():
            for eq in eqs:
                if any(x * eq[1] < 0 for x in eq[0]):
                    continue
                for i in range(len(limits)):
                    if (eq[0][i] != 0): # and eq[1] > 0) or (eq[0][1] < 0 and eq[1] < 0):
                    #if (eq[0][i] > 0 and eq[1] > 0) or (eq[0][1] < 0 and eq[1] < 0):
                    #if (eq[0][i] > 0 and eq[1] > 0) or (eq[0][1] < 0 and eq[1] < 0):
                        limits[i] = min(limits[i], int(math.ceil(eq[1] / eq[0][i])))
                        pass
        limits = [10000000000000] * len(eqs[0][0])
        find_limits()



        def find_one(item):
            one_pos = lambda x: sum(1 if y == 0 else 0 for y in x) == len(x) - 1
            #print(one_count(eqs[k]))
            if one_pos(item[0]):
                for zz, z in enumerate(item[0]):
                    if z != 0:
                        exists = any(f[0] == zz for f in frozen)
                        if exists:
                             return False

                        val = math.floor((item[1] / z) + 0.1)
                        #print(f"{zz} = {val}")
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
            mins = [next((i for i, q in enumerate(eq[0]) if q != 0), len(eqs)) for eq in eqs ]
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
                    min_pos_val = next((i for i, v in enumerate(use_row[0]) if v != 0), variable_count) # min(k if item[row] > 0 else len(eqs) for k, item in enumerate(eqs))
                    if min_pos_val >= variable_count:
                        continue
                    for k, item in enumerate(eqs):
                        if k <= row:
                            continue
                        diff = item[0][min_pos_val] / use_row[0][min_pos_val]
                        if diff == 0:
                            continue
                        item[0] = [(item[0][l] - use_row[0][l] * diff) for l in range(len(item[0]))]
                        item[1] -= diff * use_row[1]

                back_substitute()
            #return eqs

        gauss_elim()
        #eqs = gauss_elim(eqs)

        def rec():
            global eqs
            global frozen
            global limits
            global presses
            #if any(f[1] < 0 for f in frozen):
            if any(f[1] < 0 or f[1] != int(f[1]) for f in frozen):
                return
            if any(x[1] != 0 and all(y == 0 for y in x[0]) for x in eqs):
                return
            if len(frozen) != len(eqs) and any(y != 0 for x in eqs for y in x[0]):

                copy_eqs = eqs.copy()
                copy_frozen = frozen.copy()

                find_limits()
                copy_limits = limits.copy()
                smallest_limits = [[i, x] for i, x in enumerate(limits)]
                #smallest_limits = sorted(smallest_limits, key=lambda x : x[1] )

                for limit_value in smallest_limits:
                    if any(f[0] == limit_value[0] for f in frozen):
                        continue
                    for limit_range in range(limit_value[1] + 1):
                        eqs = copy_eqs.copy()
                        frozen = copy_frozen.copy()
                        limits = copy_limits.copy()
                        frozen.append([limit_value[0], limit_range])
                        back_substitute()
                        gauss_elim()
                        rec()

                        #print(f"{eqs} | {eqs[k][1]}")
                        #print(f"{eqs}")
            else:
                #if all(x[1] == 0 for x in eqs):

                values = [0] * len(jolts)
                for f in frozen:
                    for b in buttons[f[0]]:
                        values[b] += f[1]

                if all(x == jolts[iii] for iii, x in enumerate(values)):
                    new_sum = sum(x[1] for x in frozen)
                    presses = min(presses, new_sum)
            return

        rec()

        frozen = sorted(frozen, key=lambda x : x[0], reverse=True)
        #print(f"all frozen {frozen}")
        #if any(eq[1] < 0 for eq in eqs):
        #    print(f"{eqs}")
        #    has_sames()

        find_limits()

        #print(f"limits {limits}")
        #if len(frozen) == 0: #and line_num == 37:
        #    print(f"ech: {eqs}")
        for f in frozen:
            for b in buttons[f[0]]:
                jolts[b] -= f[1]
            del buttons[f[0]]
            del limits[f[0]]

        #print(f"jolts {jolts}")

        if any(j != 0 for j in jolts) and len(buttons) > 0 and False:

            tmp = list(zip(buttons, limits))

            #button_counts = [0] * len(jolts)
            #for b in tmp: #buttons[0]
            #    for j in b[0]:
            #        button_counts[j] += 1
            #tmp = sorted(tmp, key=lambda x : min(button_counts[y] for y in x[0])) # * len(x)) #, reverse=True)


            tmp = sorted(tmp, key=lambda x : x[1])
            #button_counts = [0] * len(jolts)
            #for b in tmp: #buttons[0]
            #    for j in b[0]:
            #        button_counts[j] += 1
            #tmp = sorted(tmp, key=lambda x : min(button_counts[y] for y in x[0])) # * len(x)) #, reverse=True)
            buttons = [x[0] for x in tmp]
            limits = [x[1] for x in tmp]
            #buttons = sorted(buttons, key=lambda x : len(x), reverse=True)
            #buttons = sorted(buttons, key=lambda x : min(button_counts[y] for y in x)) # * len(x)) #, reverse=True)
            #result = [c == '#' for c in l[0][1:-1]]
            #print(f"{buttons}, {result}")
            button_count = len(l[0]) - 2
            sequence_count = len(buttons)










            is_valid = lambda x: all(x[i] <= jolts[i] for i in range(len(jolts)))
            stack = []
            stack.append([0] * sequence_count)

            #b_count = values[b] += amount for j, amount in enumerate(stack[0]) for b in buttons[j] ]
            counted_jolts = [0] * len(jolts)

            def count_jolts(v):
                values = [0] * len(jolts)
                for j, amount in enumerate(stack[0]):
                    for b in buttons[j]:
                        values[b] += amount
                return values

            def add_jolts(ind, amount):
                stack[0][ind] += amount
                if amount == 0:
                    return
                for b in buttons[ind]:
                    counted_jolts[b] += amount

            def go_back(ind):
                while ind >= 1:
                    add_jolts(ind, -stack[0][ind])
                    if stack[0][ind - 1] > 0:
                        add_jolts(ind - 1, -1)
                        #if ind == 1:
                        #    print(f"going down one: {counted_jolts}")
                        break
                    else:
                        ind -= 1
                return ind


            b_min = lambda b_ind, v : min(limits[b_ind], min((jolts[b] - v[b]) for b in buttons[b_ind]))

            memoiz = set()

            get_mem = lambda ind : tuple(counted_jolts + [ind])
            t = get_mem(0)
            memoiz.update( list(get_mem(0)) )
            a = b_min(0, counted_jolts)
            add_jolts(0, a)


            #print(f"start: {counted_jolts} find: {jolts}")
            ind = 1
            f = False


            while(ind > 0): # and presses > 2**50):
                if get_mem(ind) in memoiz:
                    ind = go_back(ind)
                    continue
                memoiz.update( get_mem(ind) )
                a = b_min(ind, counted_jolts)
                if a < 0:
                    ind = go_back(ind)
                    if ind == 0:
                        break
                else:
                    add_jolts(ind, a)

                    #values = count_jolts(stack[0])
                    #if any(x != y for x,y in zip(counted_jolts, values)):
                    #    print(f"not right {list(zip(counted_jolts, values))}")
                    ind += 1
                    if ind >= sequence_count:
                        if all(x == y for x,y in zip(counted_jolts, jolts)):
                            #print(f"found one {stack[0]} presses: {sum(stack[0])}")
                            presses = min(sum(stack[0]), presses)

                        ind -= 1
                        #add_jolts(counted_jolts, ind, -stack[0][ind])
                        #stack[0][ind] = 0
                        ind = go_back(ind)
                    else:
                        remains = [jolts[i] - counted_jolts[i] > 0 for i in range(len(jolts))]
                        for indd in range(ind, sequence_count):
                            for b in buttons[indd]:
                                remains[b] = False
                        if any(remains):
                            ind -= 1
                            ind = go_back(ind)
                            #print(f"cannot continue")

        #a = b_min(1, )
        #print(f"{a} * {buttons[0]}")
        #while len(stack) > 0:
        #    s = stack[0]
        #    del stack[0]
        #    for i in range(sequence_count):
        #        s_copy = s.copy()
        #        s_copy[i] += 1
        #        values = [0] * len(jolts)
        #        for j, amount in enumerate(s_copy):
        #            for b in buttons[j]:
        #                values[b] += amount
        #        if all(x <= y for x,y in zip(values, jolts)):
        #            if all(x == y for x,y in zip(values, jolts)):
        #                print(f"found one {s_copy} presses: {sum(s_copy)}")
        #                presses = min(sum(s_copy), presses)
        #            else:
        #                if not visited.__contains__(s_copy):
        #                    stack.append(s_copy)
        #                    visited.append(s_copy)
        #        #else:
        #        #    print(f"over")


        #else:
        #    presses = 0

        #presses += sum(x[1] for x in frozen)
        end_time = timer()

        #print(f"{line_num + 1}: lowest button {presses} {stack[0]} frozen_count: {len(frozen)} dur: {timedelta(seconds=end_time - start_time)}")
        #print(f"{line_num + 1}: lowest button {presses} {frozen} frozen_count: {len(frozen)} dur: {timedelta(seconds=end_time - start_time)}")
        #print(f"{line_num + 1}: lowest button {presses} dur: {timedelta(seconds=end_time - start_time)}")
        print(f"{line_num + 1}: {presses}")
        button_presses_total += presses
        #button_presses_total += presses
    print(f"10b - Button presses: {button_presses_total} run time: {timer() - total_start_time}")
















def bbb():
    button_presses_total = 0
    lines = [line.strip().split(' ') for line in open(file_path, "r").readlines()]
    visited = []
    for l in lines:
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        jolts = list(map(int, l[-1][1:-1].split(',')))

        button_counts = [0] * len(jolts)
        for b in buttons:
            for j in b:
                button_counts[j] += 1

        buttons = sorted(buttons, key=lambda x : len(x), reverse=True)
        buttons = sorted(buttons, key=lambda x : min(button_counts[y] for y in x)) # * len(x)) #, reverse=True)
        result = [c == '#' for c in l[0][1:-1]]
        #print(f"{buttons}, {result}")
        button_count = len(l[0]) - 2
        sequence_count = len(l) - 2
        if sequence_count != len(buttons):
            print("aisjfi")

        is_valid = lambda x: all(x[i] <= jolts[i] for i in range(len(jolts)))
        stack = []
        stack.append([0] * sequence_count)

        #b_count = values[b] += amount for j, amount in enumerate(stack[0]) for b in buttons[j] ]
        counted_jolts = [0] * len(jolts)

        def count_jolts(v):
            values = [0] * len(jolts)
            for j, amount in enumerate(stack[0]):
                for b in buttons[j]:
                    values[b] += amount
            return values

        def add_jolts(ind, amount):
            stack[0][ind] += amount
            if amount == 0:
                return
            for b in buttons[ind]:
                counted_jolts[b] += amount

        def go_back(ind):
            while ind >= 1:
                add_jolts(ind, -stack[0][ind])
                if stack[0][ind - 1] > 0:
                    add_jolts(ind - 1, -1)
                    if ind == 1:
                        print(f"going down one: {counted_jolts}")
                    break
                else:
                    ind -= 1
            return ind


        b_min = lambda b_ind, v : min((jolts[b] - v[b] for b in buttons[b_ind]))


        a = b_min(0, counted_jolts)
        add_jolts(0, a)


        #print(f"start: {counted_jolts} find: {jolts}")
        ind = 1
        f = False


        while(ind > 0 and presses > 2**50):
            a = b_min(ind, counted_jolts)
            if a < 0:
                ind = go_back(ind)
                if ind == 0:
                    break
            else:
                add_jolts(ind, a)
                #values = count_jolts(stack[0])
                #if any(x != y for x,y in zip(counted_jolts, values)):
                #    print(f"not right {list(zip(counted_jolts, values))}")
                ind += 1
                if ind >= sequence_count:
                    if all(x == y for x,y in zip(counted_jolts, jolts)):
                        print(f"found one {stack[0]} presses: {sum(stack[0])}")
                        presses = min(sum(stack[0]), presses)

                    ind -= 1
                    #add_jolts(counted_jolts, ind, -stack[0][ind])
                    #stack[0][ind] = 0
                    ind = go_back(ind)
                else:
                    remains = [jolts[i] - counted_jolts[i] > 0 for i in range(len(jolts))]
                    for indd in range(ind, sequence_count):
                        for b in buttons[indd]:
                            remains[b] = False
                    if any(remains):
                        ind -= 1
                        ind = go_back(ind)
                        #print(f"cannot continue")

        #a = b_min(1, )
        #print(f"{a} * {buttons[0]}")
        #while len(stack) > 0:
        #    s = stack[0]
        #    del stack[0]
        #    for i in range(sequence_count):
        #        s_copy = s.copy()
        #        s_copy[i] += 1
        #        values = [0] * len(jolts)
        #        for j, amount in enumerate(s_copy):
        #            for b in buttons[j]:
        #                values[b] += amount
        #        if all(x <= y for x,y in zip(values, jolts)):
        #            if all(x == y for x,y in zip(values, jolts)):
        #                print(f"found one {s_copy} presses: {sum(s_copy)}")
        #                presses = min(sum(s_copy), presses)
        #            else:
        #                if not visited.__contains__(s_copy):
        #                    stack.append(s_copy)
        #                    visited.append(s_copy)
        #        #else:
        #        #    print(f"over")
        print(f"lowest button {presses}")
        button_presses_total += presses
        #button_presses_total += presses
    print(f"10a - Button presses: {button_presses_total}")

def bb():
    button_presses_total = 0
    lines = [line.strip().split(' ') for line in open(file_path, "r").readlines()]
    visited = []
    for l in lines:
        presses = 2**60
        buttons = [list(map(int, line[1:-1].split(','))) for line in l[1:-1]]
        buttons = sorted(buttons, key=lambda x : len(x), reverse=True)
        jolts = list(map(int, l[-1][1:-1].split(',')))
        result = [c == '#' for c in l[0][1:-1]]
        #print(f"{buttons}, {result}")
        button_count = len(l[0]) - 2
        sequence_count = len(l) - 2

        is_valid = lambda x: all(x[i] <= jolts[i] for i in range(len(jolts)))
        stack = []
        stack.append([0] * sequence_count)
        while len(stack) > 0:
            s = stack[0]
            del stack[0]
            for i in range(sequence_count):
                s_copy = s.copy()
                s_copy[i] += 1
                values = [0] * len(jolts)
                for j, amount in enumerate(s_copy):
                    for b in buttons[j]:
                        values[b] += amount
                if all(x <= y for x,y in zip(values, jolts)):
                    if all(x == y for x,y in zip(values, jolts)):
                        print(f"found one {s_copy} presses: {sum(s_copy)}")
                        presses = min(sum(s_copy), presses)
                    else:
                        if not visited.__contains__(s_copy):
                            stack.append(s_copy)
                            visited.append(s_copy)
                #else:
                #    print(f"over")
        print(f"lowest button {presses}")
        button_presses_total += presses
    print(f"10a - Button presses: {button_presses_total}")

a()#
b()




