const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

//pub fn day5(alloc: *std.mem.Allocator, comptime inputFileName: []const u8 ) anyerror!void
const boardSize: u32 = 1024;
var lineBoard : [boardSize * boardSize]u8 = undefined;

pub fn day5(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    lineBoard = std.mem.zeroes([boardSize * boardSize]u8);

    var overLappingPoints: u32 = 0;
    var overLappingPoints2: u32 = 0;

    var lines = std.mem.tokenize(u8, inputFile, "\r\n");
    while (lines.next()) |line|
    {
        var charIndex: u32 = 0;
        const x1 = getNumber(line, &charIndex);
        charIndex += 1;
        const y1 = getNumber(line, &charIndex);

        charIndex += 4;
        const x2 = getNumber(line, &charIndex);
        charIndex += 1;
        const y2 = getNumber(line, &charIndex);

        const dx:u32 = distance(x2, x1);
        const dy:u32 = distance(y2, y1);

        if(dx == 0 or dy == 0)
        {
            const xmin = @minimum(x1, x2);
            const ymin = @minimum(y1, y2);
            const startInd = getIndex(xmin, ymin);

            var i:u32 = 0;
            while(i <= dx) : (i += 1)
            {
                const ind = startInd + i;

                // seems to be about same, maybe about same code?
                // when splitting horizontal and vertical pass, creates
                // slightly faster code?, cos probably can simd better,
                // and less issues with cache.
                const adds = (~lineBoard[ind]) & (32 + 2);
                lineBoard[ind] += adds >> 1;
                const boardAnd = lineBoard[ind] & adds;
                overLappingPoints += (boardAnd >> 1) & 1;
                overLappingPoints2 += boardAnd >> 5;

                //if(lineBoard[ind] & 2 == 0)
                //{
                //    lineBoard[ind] += 1;
                //    overLappingPoints += ((lineBoard[ind] >> 1) & 1);
                //}
                //if(lineBoard[ind] & 32 == 0)
                //{
                //    lineBoard[ind] += 16;
                //    overLappingPoints2 += (lineBoard[ind] >> 5);
                //}
            }
            while(i <= dy) : (i += 1)
            {
                const ind = startInd + boardSize * i;

                // seems to be about same, maybe about same code?
                const adds = (~lineBoard[ind]) & (32 + 2);
                lineBoard[ind] += adds >> 1;
                const boardAnd = lineBoard[ind] & adds;
                overLappingPoints += (boardAnd >> 1) & 1;
                overLappingPoints2 += boardAnd >> 5;
            }
        }
        else if(dx == dy)
        {
            const dirX = direction(x1, x2);
            const dirY = direction(y1, y2);

            const startInd = @intCast(i32, getIndex(x1, y1));
            const movement = dirX + dirY * @intCast(i32, boardSize);

            var i:i32 = 0;
            while(i <= dx) : (i += 1)
            {
                const ind = @intCast(u32, startInd + movement * i);

                // seems to be a bit slower
                //const adds = (~lineBoard[ind]) & 32;
                //lineBoard[ind] += adds >> 1;
                //overLappingPoints2 += ((lineBoard[ind] & adds) >> 5);

                if(lineBoard[ind] & 32 == 0)
                {
                    lineBoard[ind] += 16;
                    overLappingPoints2 += (lineBoard[ind] >> 5);
                }

            }

        }

    }

    const res = try std.fmt.bufPrint(printBuffer, "Day5-1: Overlapping points: {d}\n", .{overLappingPoints});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day5-2: Overlapping points: {d}\n", .{overLappingPoints2});
    return res.len + res2.len;
}

fn getIndex(x: u32, y:u32) u32
{
    return x + y * boardSize;
    //const xBorder:u32 = x >> 5;
    //const yBorder:u32 = y >> 5;
    //const borderInd: u32 = xBorder + yBorder * 32; // 1024 / 32 = 32
    //return (x & 31) + (y & 31) * 8 + borderInd * 1024;
}

fn getNumber(line: []const u8, ind: *u32) u32
{
    var result: u32 = 0;
    while(ind.* < line.len) : (ind.* += 1)
    {
        const c: u8 = line[ind.*];
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

fn distance(a: u32, b: u32) u32
{
    return if(a > b) a - b else b - a;
}

fn direction(a: u32, b: u32) i32
{
    return if(a > b) -1 else 1;
}