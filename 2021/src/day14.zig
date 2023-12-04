const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day14(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var rules = std.mem.zeroes([128 * 3]u8);
    var rulesCount: u32 = 0;

    // Count for every depth in every rule for every single letter
    var counts = std.mem.zeroes([50][128][16]u64);

    // to determine for example AB -> C, needs AC and CB, so no need
    // to search it more than once.
    var transforms = std.mem.zeroes([128][2]u8);

    // starting string
    var startString = std.mem.zeroes([64]u8);

    // remapping unique letters
    var remapTable = std.mem.zeroes([32]u8);


    // Parse and remap characters to values.
    var uniqueChars: u32 = 0;
    var strLen: usize = 0;
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        const ruleStr = lines.next().?;
        strLen = ruleStr.len;
        std.mem.copy(u8, &startString, ruleStr);

        remapChars(&startString, &remapTable, strLen, &uniqueChars);

        while (lines.next()) |line|
        {
            var rulesIter = std.mem.tokenize(u8, line, " -> ");
            const left = rulesIter.next().?;
            const right = rulesIter.next().?;

            rules[rulesCount * 3 + 0] = left[0];
            rules[rulesCount * 3 + 1] = left[1];
            rules[rulesCount * 3 + 2] = right[0];

            remapChars(rules[rulesCount * 3..rulesCount * 3 + 3], &remapTable, 3, &uniqueChars);
            rulesCount += 1;
        }
    }
    // Build quick access transform lookup table, so no need to search
    // index on every iteration of adding values.
    {
        var i: u32 = 0;
        while(i < rulesCount) : (i += 1)
        {
            const l = rules[i * 3 + 0];
            const r = rules[i * 3 + 1];
            const m = rules[i * 3 + 2];

            counts[0][i][l] += 1;
            counts[0][i][r] += 1;

            var j: u32 = 0;
            var found: u32 = 0;
            while(j < rulesCount) : (j += 1)
            {
                if(rules[j * 3 + 0] == l and rules[j * 3 + 1] == m)
                {
                    transforms[i][0] = @intCast(u8, j);
                    found += 1;
                }
                if(rules[j * 3 + 0] == m and rules[j * 3 + 1] == r)
                {
                    transforms[i][1] = @intCast(u8, j);
                    found += 1;
                }
            }
            if(found != 2)
            {
                print("Didn't find some transformtable for: {}\n", .{i});
            }
        }
    }
    // Sum all characters for every step up to 40 included.
    {
        var i: u32 = 1;
        while(i <= 40) : (i += 1)
        {
            var j: u32 = 0;
            while(j < rulesCount) : (j += 1)
            {
                var k: u32 = 0;
                while(k < uniqueChars) : (k += 1)
                {
                    // add the subtree counts.
                    counts[i][j][k] += counts[i - 1][transforms[j][0]][k];
                    counts[i][j][k] += counts[i - 1][transforms[j][1]][k];
                }

                // the middle one gets added twice
                const middleIndex = @intCast(usize, rules[j * 3 + 2]);
                counts[i][j][middleIndex] -= 1;
            }
        }

    }

    var resultA: u64 = 0;
    var resultB: u64 = 0;
    // Finally calculate sum of characters of the original string.
    {
        var countsAt10 = std.mem.zeroes([16]u64);
        var countsAt40 = std.mem.zeroes([16]u64);

        var i: u32 = 0;
        while(i + 1 < strLen) : (i += 1)
        {
            // find the left index.
            const l = startString[i];
            const r = startString[i + 1];

            var j: u32 = 0;
            while(j < rulesCount) : (j += 1)
            {
                if(rules[j * 3 + 0] == l and rules[j * 3 + 1] == r)
                {
                    var k: u32 = 0;
                    while(k < uniqueChars) : (k += 1)
                    {
                        countsAt10[k] += counts[10][j][k];
                        countsAt40[k] += counts[40][j][k];
                    }

                    if(i + 2 < strLen)
                    {
                        countsAt10[r] -= 1;
                        countsAt40[r] -= 1;
                    }
                    break;
                }
            }
        }
        resultA = getMaxMinDiff(&countsAt10, uniqueChars);
        resultB = getMaxMinDiff(&countsAt40, uniqueChars);
   }

    const res =try std.fmt.bufPrint(printBuffer, "Day 14-1: max - min diff: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 14-2: max - min diff: {}\n", .{resultB});
    return res.len + res2.len;
}

fn remapChars(stringToRemap: []u8, remapTable: []u8, strLen: usize, uniqueChars: *u32) void
{
    var i: u32 = 0;
    while(i < strLen) : (i += 1)
    {
        const c: u8 = stringToRemap[i];
        var j: u32 = 0;
        while(j < uniqueChars.*) : (j += 1)
        {
            if(c == remapTable[j])
            {
                break;
            }
        }
        if(j == uniqueChars.*)
        {
            remapTable[j] = c;
            uniqueChars.* += 1;
        }
        stringToRemap[i] = @intCast(u8, j);
    }
}

fn getMaxMinDiff(chars: []const u64, count: u32) u64
{
    var minAmount: u64 = 0xffff_ffff_ffff_ffff;
    var maxAmount: u64 = 0;

    var i: u32 = 0;
    while(i < count) : (i += 1)
    {
        const minTmp = @minimum(minAmount, chars[i]);
        minAmount = if(minTmp > 0) minTmp else minAmount;
        maxAmount = @maximum(maxAmount, chars[i]);
    }
    return maxAmount - minAmount;
}