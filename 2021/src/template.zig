const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day21(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;
    //var x = try std.fmt.parseInt(i32, numbers.next().?, 10);
    //var matches = std.ArrayList(u32).init(alloc);
    //defer matches.deinit();
    //try matches.append(Pair{.first = ind1, .second = ind2});

    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");

        while(lines.next()) |line|
        {
            print("{s}\n", .{line});
        }
    }



    const res =try std.fmt.bufPrint(printBuffer, "Day 21-1: Beacons: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 21-2: Biggest manhattan distance: {}\n", .{resultB});
    return res.len + res2.len;
}

