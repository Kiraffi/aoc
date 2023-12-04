const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

//pub fn day6(alloc: *std.mem.Allocator, comptime inputFileName: []const u8 ) anyerror!void
pub fn day6(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    //const inputFile = @embedFile(inputFileName);
    var lines = std.mem.tokenize(u8, inputFile, "\r\n");
    var numbersIter = std.mem.tokenize(u8, lines.next().?, ",");

    var spawns: [9]u64 = std.mem.zeroes([9]u64);

    while (numbersIter.next()) |numberString|
    {
        const num = try std.fmt.parseInt(u64, numberString, 10);
        spawns[num] += 1;
    }

    const res = try std.fmt.bufPrint(printBuffer, "Day6-1: Fish Count: {d}\n", .{simulate(spawns, 80)});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day6-2: Fish Count: {d}\n", .{simulate(spawns, 256)});
    return res.len + res2.len;
}

// making copy of the input counts, sadly spawninput becomes constant?
fn simulate(spawnInput: [9]u64, days: u64) u64
{
    var spawns: [9]u64 = spawnInput;
    var day:u32 = 0;
    while(day < days) : (day += 1)
    {
        spawns[7] += spawns[0];
        std.mem.rotate(u64, &spawns, 1);
    }

    var sum: u64 = 0;
    var i: u32 = 0;
    while (i < 9) : (i += 1)
    {
        sum += spawns[i];
    }
    return sum;
}