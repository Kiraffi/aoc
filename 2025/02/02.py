import os
dir_path = os.path.dirname(os.path.realpath(__file__)) + "/"

file_path = dir_path + "data.txt"

def parse_values():
    values = []
    with open(file_path, "r") as file:
        for line in file:
            numbers = line.split(',')
            for num in numbers:
                s = num.split('-')
                values.append((int(s[0].strip()), int(s[1].strip())))
    values = sorted(values, key = lambda num : num[0])
    return values

def a():
    values = parse_values()

    get_double_value = lambda v : int(str(v) + str(v))

    def get_next_value(value):
        value += 1
        double_value = get_double_value(value)
        return value, double_value

    sum_invalid_ids = 0

    value = 1
    double_value = get_double_value(value)
    index = 0
    len_values = len(values)

    while index < len_values:
        if double_value < values[index][0]:
            value, double_value = get_next_value(value)
        elif double_value > values[index][1]:
            index += 1
        else:
            sum_invalid_ids += double_value
            value, double_value = get_next_value(value)

    print(f"2a - Sum of invalid ids: {sum_invalid_ids}")


def b():
    values = parse_values()

    def get_multiple_value(value, mult):
        result = ""
        for i in range(mult):
            result += str(value)

        return int(result)

    def get_next_value(value, mult):
        value += 1
        multi_value = get_multiple_value(value, mult)
        return value, multi_value

    sum_invalid_ids = 0

    mult = 2
    hits = []
    while mult <= 10:
        value = 1
        multi_value = get_multiple_value(value, mult)

        index = 0
        len_values = len(values)

        while index < len_values:
            if multi_value < values[index][0]:
                value, multi_value = get_next_value(value, mult)
            elif multi_value > values[index][1]:
                index += 1
            else:
                if not multi_value in hits:
                    sum_invalid_ids += multi_value
                    hits.append(multi_value)
                value, multi_value = get_next_value(value, mult)

        mult += 1
    print(f"2b - Sum of invalid ids: {sum_invalid_ids}")

a()
b()
