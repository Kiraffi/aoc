const std = @import("std");

const print = std.debug.print;

const fns = .{
    @import("day1.zig").day1,
    @import("day2.zig").day2,
    @import("day3.zig").day3,
    @import("day4.zig").day4,
    @import("day5.zig").day5,
    @import("day6.zig").day6,
    @import("day7.zig").day7,
    @import("day8.zig").day8,
    @import("day9.zig").day9,
    @import("day10.zig").day10,
    @import("day11.zig").day11,
    @import("day12.zig").day12,
    @import("day13.zig").day13,
    @import("day14.zig").day14,
    @import("day15.zig").day15,
    @import("day16.zig").day16,
    @import("day17.zig").day17,
    @import("day18.zig").day18,
    @import("day19.zig").day19,
    @import("day20.zig").day20,
    @import("day21.zig").day21,
    @import("day22.zig").day22,
    @import("day23.zig").day23,
    @import("day24.zig").day24,
    @import("day25.zig").day25,
};

pub fn main() anyerror!void
{
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    var totalTimer : std.time.Timer =  try std.time.Timer.start();
    defer print("Total dur: {}us\n\n", .{totalTimer.read() / 1000});

    const allocator = &arena.allocator;
    const loopAmount: u32 = 1;

    try printDay(allocator, "input_day1.txt", 1, loopAmount);
    try printDay(allocator, "input_day2.txt", 2, loopAmount);
    try printDay(allocator, "input_day3.txt", 3, loopAmount);
    try printDay(allocator, "input_day4.txt", 4, loopAmount);
    try printDay(allocator, "input_day5.txt", 5, loopAmount);
    try printDay(allocator, "input_day6.txt", 6, loopAmount);
    try printDay(allocator, "input_day7.txt", 7, loopAmount);
    try printDay(allocator, "input_day8.txt", 8, loopAmount);
    try printDay(allocator, "input_day9.txt", 9, loopAmount);
    try printDay(allocator, "input_day10.txt", 10, loopAmount);
    try printDay(allocator, "input_day11.txt", 11, loopAmount);
    try printDay(allocator, "input_day12.txt", 12, loopAmount);
    try printDay(allocator, "input_day13.txt", 13, loopAmount);
    try printDay(allocator, "input_day14.txt", 14, loopAmount);
    try printDay(allocator, "input_day15.txt", 15, loopAmount);
    try printDay(allocator, "input_day16.txt", 16, loopAmount);
    try printDay(allocator, "input_day17.txt", 17, loopAmount);
    try printDay(allocator, "input_day18.txt", 18, loopAmount);
    try printDay(allocator, "input_day19.txt", 19, loopAmount);
    try printDay(allocator, "input_day20.txt", 20, loopAmount);
    try printDay(allocator, "input_day21.txt", 21, loopAmount);
    try printDay(allocator, "input_day22.txt", 22, loopAmount);
    try printDay(allocator, "input_day23.txt", 23, loopAmount);
    try printDay(allocator, "input_day24.txt", 24, loopAmount);
    try printDay(allocator, "input_day25.txt", 25, loopAmount);
}

fn printDay(allocator: *std.mem.Allocator, comptime inputFileName: []const u8,
    comptime dayNum: u32, loopAmount: u32) anyerror!void
{
    if(dayNum > fns.len)
        return;
    var minV: u64 = 0xffff_ffff_ffff_ffff;
    var maxV: u64 = 0;

    //const inputFile: []const u8 = @embedFile("../" ++ inputFileName);

    const inputFile : []u8 = try std.fs.cwd().readFileAlloc(allocator, inputFileName, std.math.maxInt(usize));
    defer allocator.free(inputFile);

    var printBuffer: [1024]u8 = undefined;

    var timer : std.time.Timer =  try std.time.Timer.start();
    var i:u32 = 0;

    var strLen: usize = 0;
    while(i < loopAmount) : (i += 1)
    {
        printBuffer = std.mem.zeroes([1024]u8);

        timer.reset();
        strLen = try fns[dayNum - 1](allocator, inputFile, &printBuffer);
        const t = timer.read() / (1000);
        minV = @minimum(minV, t);
        maxV = @maximum(maxV, t);
    }
    print("{s}Day{} dur: {}-{}us (min-max), in {} executions\n\n", .{printBuffer[0..strLen], dayNum, minV, maxV, loopAmount});
}