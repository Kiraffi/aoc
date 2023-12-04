const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const Point = struct {
    x: u32,
    y: u32,
};


pub fn day9(alloc: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var lowestPoints = std.ArrayList(Point).init(alloc);
    defer lowestPoints.deinit();

    var printLen: usize = 0;

     var board: [102 * 102]u8 = std.mem.zeroes([102 * 102]u8);
    {
        var i:u32 = 0;
        while(i < 102) : (i += 1)
        {
            board[i] = 10;
            board[i + 101 * 102] = 10;
            board[i * 102] = 10;
            board[101 + i * 102] = 10;
        }
    }

    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        // Parse
        var lineNum: u32 = 1;
        while (lines.next()) |line|
        {
            var i:u32 = 0;
            var index:u32 = lineNum * 102 + 1;
            while(i < line.len) : (i += 1)
            {
                var num:u8 = line[i] - '0';
                board[index] = num;
                index += 1;
            }
            lineNum += 1;
        }
    }
    const boardHeight:u64 = 102;
    const boardWidth: u64 = 102;
    {
        // Find minimums
        var sumOfMins: u32 = 0;
        var y:u32 = 1;
        while(y < boardHeight - 1) : (y += 1)
        {
            var x:u32 = 1;
            while(x < boardWidth - 1) : (x += 1)
            {
                const curr:u8 = board[x + y * boardWidth];
                const lowestNeighbour: u8 = @minimum(
                    @minimum(board[x - 1 + y * boardWidth], board[x + 1 + y * boardWidth]),
                    @minimum(board[x + (y - 1) * boardWidth], board[x + (y + 1) * boardWidth]));

                if(curr < lowestNeighbour)
                {
                    sumOfMins += @intCast(u32, curr + 1);
                    try lowestPoints.append(Point{.x = x, .y = y});
                }
            }
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day9-1: Sum of mins: {}\n", .{sumOfMins});
        printLen = res.len;
    }

    {
        var tiles = std.ArrayList(u32).init(alloc);
        defer tiles.deinit();
        var i:u32 = 0;
        while(i < lowestPoints.items.len) : (i += 1)
        {
            try tiles.append( fill(&board, lowestPoints.items[i].x, lowestPoints.items[i].y) );
        }
        std.sort.sort(u32, tiles.items, {}, comptime std.sort.desc(u32));

        const res = try std.fmt.bufPrint(printBuffer[printLen..], "Day9-2: Basins {}\n", .{ tiles.items[0] * tiles.items[1] * tiles.items[2] });
        return res.len + printLen;
    }
}


fn fill(board: []u8, x: u32, y: u32) u32
{
    const ind: u32 = x + y * 102;
    if(board[ind] >= 9)
        return 0;
    board[ind] = 10;
    var tiles: u32 = 1;
    tiles += fill(board, x - 1, y);
    tiles += fill(board, x + 1, y);
    tiles += fill(board, x, y - 1);
    tiles += fill(board, x, y + 1);
    return tiles;
}