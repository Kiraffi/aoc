//! By convention, main.zig is where your main function lives in the case that
//! you are building an executable. If you are making a library, the convention
//! is to delete this file and start with root.zig instead.
const std = @import("std");

const print = std.debug.print;
// std.mem.sort

const aInput = @embedFile("01.input");

pub fn main() !void
{
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();

    const allocator = arena.allocator();

    var leftList = std.ArrayList(i32).init(allocator);
    defer leftList.deinit();

    var rightList = std.ArrayList(i32).init(allocator);
    defer rightList.deinit();

    var it = std.mem.tokenizeAny(u8, aInput, "\n ");
    var left = true;
    while (it.next()) |num|
    {
        const n = try std.fmt.parseInt(i32, num, 10);
        if(left)
        {
            try leftList.append(n);
        }
        else
        {
            try rightList.append(n);
        }
        left = !left;
    }

    std.mem.sort(i32, leftList.items, {}, std.sort.asc(i32));
    std.mem.sort(i32, rightList.items, {}, std.sort.asc(i32));
    d01a(leftList.items, rightList.items);
    d01b(leftList.items, rightList.items);
}

pub fn d01a(leftItems: [] const i32, rightItems: []const i32) void
{
    var diff: u32 = 0;
    for (leftItems, rightItems) |l, r|
    {
        diff += @abs(l - r);
    }
    print("01-a differece: {}\n", .{diff});
}

pub fn d01b(leftItems: [] const i32, rightItems: []const i32) void
{
    var diff: i64 = 0;
    for (leftItems) |l|
    {
        var count: i64 = 0;
        for(rightItems) |r|
        {
            if(l == r)
            {
                count += 1;
            }
        }
        diff += count * @as(i64, l);
    }
    print("01-b simularity: {}\n", .{diff});
}


