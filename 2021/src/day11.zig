const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day11(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    // (1 + 1 << 6) + (1 << 12) + (1 << 18) + (1 << 24)
    //+ (1 << 30 )+ (1 << 36 ) + (1 << 42) + (1 << 48) + (1 << 54)
    // = 18300341342965825 = 0x41041041041041
    const firstBitAll: u64 = 0x41041041041041;

    var board = std.mem.zeroes([10]u64);
    var flashTable = std.mem.zeroes([10]u64);
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        var y:u32 = 0;
        while (lines.next()) |line|
        {
            var x: u32 = 0;
            while(x < line.len) : (x += 1)
            {
                const index:u32 = 6 * x;
                board[y] |= @intCast(u64, (line[x] - '0') + 6) << @intCast(u6, index);
            }
            y += 1;
        }
    }

    {
        var steps: u32 = 0;
        var allFlashStep: u32 = 0;
        var flashesAt100: u32 = 0;
        while(steps < 100 or allFlashStep == 0) : (steps += 1)
        {
            var y:u32 = 0;
            while(y < 10) : (y += 1)
            {
                board[y] += firstBitAll;
            }
            var newFlashes: u64 = 1;
            while(newFlashes != 0)
            {
                newFlashes = 0;

                // 5th bit set.
                const flashMask: u64 = firstBitAll << 4;

                var prevFlashRow: u64 = 0;
                var currFlashRow = (board[0] & flashMask) & (~flashTable[0]);
                var nextFlashRow = (board[1] & flashMask) & (~flashTable[1]);

                y = 0;
                while(y < 9) : (y += 1)
                {
                    nextFlashRow = (board[y + 1] & flashMask) & (~flashTable[y + 1]);
                    const scrollingFlashRow = (prevFlashRow + currFlashRow + nextFlashRow) >> 4;

                    newFlashes |= scrollingFlashRow;

                    var addValue: u64 = scrollingFlashRow;
                    addValue += (scrollingFlashRow << 6);
                    addValue += (scrollingFlashRow >> 6);
                    _ = @addWithOverflow(u64, board[y], addValue, &board[y]);

                    flashTable[y] |= currFlashRow;
                    prevFlashRow = currFlashRow;
                    currFlashRow = nextFlashRow;
                }

                const scrollingFlashRow = (prevFlashRow + currFlashRow) >> 4;

                var addValue: u64 = scrollingFlashRow;
                addValue += (scrollingFlashRow << 6);
                addValue += (scrollingFlashRow >> 6);
                _ = @addWithOverflow(u64, board[9], addValue, &board[9]);

                flashTable[9] |= currFlashRow;


                newFlashes |= scrollingFlashRow;
            }

            y = 0;
            var flashCountPerStep: u32 = 0;
            while(y < 10) : (y += 1)
            {
                flashCountPerStep += @popCount(u64, flashTable[y]);

                // reset exploded ones to 6
                // andMask ~6 for exploded
                const andMask = flashTable[y] | flashTable[y] >> 1 | flashTable[y] >> 4 | flashTable[y] << 1;
                const orMask = flashTable[y] >> 2 | flashTable[y] >> 3;

                board[y] &= ~andMask;
                board[y] |= orMask;

                // clear 5 upper bits for sure
                board[y] &= @as(u64, 0x07ff_ffff_ffff_ffff);
                flashTable[y] = 0;
            }

            if(flashCountPerStep == 100 and allFlashStep == 0)
            {
                allFlashStep = steps;
            }
            if(steps < 100)
            {
                flashesAt100 += flashCountPerStep;
            }
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day11-1: Flashes: {}\n", .{flashesAt100});
        const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day11-2: All flash step: {}\n", .{allFlashStep + 1});
        return res.len + res2.len;
    }
}
