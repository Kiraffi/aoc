const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

pub fn day2(_: *std.mem.Allocator, inputFile : []const u8, printBuffer: []u8) anyerror! usize
{
    //const inputFile : []u8 = try std.fs.cwd().readFileAlloc(alloc, "input_day2.txt", std.math.maxInt(usize) );
    //defer alloc.free(file_string);

    var lines = std.mem.tokenize(u8, inputFile, "\r\n");

    var forward: i32 = 0;
    // depth becomes aim for 2-2
    var depth: i32 = 0;
    var depthFromAim: i32 = 0;
    while (lines.next()) |line|
    {
        var line_iter = std.mem.tokenize(u8, line, " ");
        const movementDir = line_iter.next().?;
        const number_iter = line_iter.next().?;
        const num:i32 = @intCast(i32, number_iter[0] - '0');
        if(std.mem.eql(u8, movementDir, "forward"))
        {
            forward += num;
            depthFromAim += depth * num;
        }
        if(std.mem.eql(u8, movementDir, "down"))
        {
            depth += num;
        }
        if(std.mem.eql(u8, movementDir, "up"))
        {
            depth -= num;
        }
    }

    const res = try std.fmt.bufPrint(printBuffer[0..100], "Day2-1: forward:{d}, depth:{d}, mul:{d}\n", .{forward, depth, forward * depth});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day2-2: forward:{d}, depth:{d}, mul:{d}\n", .{forward, depthFromAim, forward * depthFromAim});
    return res.len + res2.len;
}
