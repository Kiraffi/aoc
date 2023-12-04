const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


fn isNumber(c: u8) bool
{
    return c >= '0' and c <= '9';
}

fn parseNumber(s: []const u8, ind: *u32) u32
{
    var result: u32 = 0;
    while(ind.* < s.len) : (ind.* += 1)
    {
        const c = s[ind.*];
        if(c >= '0' and c <= '9')
        {
            result = result * 10 + c - '0';
        }
        else
        {
            return result;
        }
    }
    return result;
}


fn getFoldedValue(x: u32, minX: u32) u32
{
    var v = x;
    v -= v / (minX + 1);
    v %= 2 * minX;
    if(v >= minX) v = 2 * minX - 1 - v;
    return v;
}

const ORIG_MAPSIZE: u32 = 2000;

const INPUTS: u32 = 2000;
//var inputs: [INPUTS]u32 = undefined;

const MAPSIZE2: u32 = 50;
//var map2: [MAPSIZE2 * MAPSIZE2]u8 = undefined;

pub fn day13(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var printLen: usize = 0;

    var inputs: [INPUTS]u32 =  std.mem.zeroes([INPUTS]u32);
    var map2: [MAPSIZE2 * MAPSIZE2]u8 = std.mem.zeroes([MAPSIZE2 * MAPSIZE2]u8);

    var inputCountA: u32 = 0;
    var minX: u32 = 0xffff_ffff;
    var minY: u32 = 0xffff_ffff;

    var firstX: u32 = 0;
    var firstY: u32 = 0;

    // Parsing
    {
        var i: u32 = @intCast(u32, inputFile.len - 1);
        while(i > 0) : (i -= 1)
        {
            const c = inputFile[i];
            if(c == 'x')
            {
                var j = i + 2;
                const parsedValue = parseNumber(inputFile, &j);
                minX = @minimum(minX, parsedValue);

                firstX = parsedValue;
                firstY = 0;
            }
            if(c == 'y')
            {
                var j = i + 2;
                const parsedValue = parseNumber(inputFile, &j);
                minY = @minimum(minY, parsedValue);

                firstX = 0;
                firstY = parsedValue;
            }
            if(c == ',')
                break;
        }
        firstX = if(firstX == 0) ORIG_MAPSIZE else firstX;
        firstY = if(firstY == 0) ORIG_MAPSIZE else firstY;

        i = 0;
        while(i < inputFile.len) : (i += 1)
        {
            const c = inputFile[i];
            if(c == 'f')
                break;
            if(c >= '0' and c <= '9')
            {
                const x = parseNumber(inputFile, &i);
                i += 1;
                const y = parseNumber(inputFile, &i);

                const map1Index = getFoldedValue(x, firstX) + getFoldedValue(y, firstY) * ORIG_MAPSIZE;
                inputs[inputCountA] = map1Index;
                inputCountA += 1;

                // part b
                map2[getFoldedValue(x, minX) + getFoldedValue(y, minY) * MAPSIZE2] = 1;
            }
        }
    }

    {
        std.sort.sort(u32, &inputs, {}, comptime std.sort.desc(u32));
        var prev: u32 = inputs[0];
        var count: u32 = 1;
        var index: u32 = 1;
        while(index < inputCountA) : (index += 1)
        {
            if(inputs[index] != prev)
                count += 1;
            prev = inputs[index];
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day13-1: Dots(hashes): {}\n", .{count});
        printLen = res.len;
    }

    // Printing part 2
    {
        const res = try std.fmt.bufPrint(printBuffer[printLen..], "Day13-2: Secret message:\n", .{});
        printLen += res.len;

        var y:u32 = 0;
        while(y < minY) : (y += 1)
        {
            var x: u32 = 0;
            while(x < minX) : (x += 1)
            {
                const c: u8 = if(map2[x + y * MAPSIZE2] == 1) '#' else '.';
                printBuffer[printLen] = c;
                printLen += 1;
            }
            printBuffer[printLen] = '\n';
            printLen += 1;
        }
    }
    return printLen;
}
