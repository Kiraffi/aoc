const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day8(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var printLen: usize = 0;
    {
        var nums: [10]u64 = std.mem.zeroes([10]u64);
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");

        while (lines.next()) |line|
        {
            var splits = std.mem.tokenize(u8, line, "|");
            _ = splits.next().?;
            const rightSide = splits.next().?;
            var numTexts = std.mem.tokenize(u8, rightSide, " ");
            while(numTexts.next()) | numText |
            {
                if(numText.len == 2) nums[1] += 1;
                if(numText.len == 4) nums[4] += 1;
                if(numText.len == 3) nums[7] += 1;
                if(numText.len == 7) nums[8] += 1;
            }
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day8-1: 1,4,7,8 appears: {} times\n", .{nums[1] + nums[4] + nums[7] + nums[8]});
        printLen = res.len;
    }
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        var numberSum: u64 = 0;
        while (lines.next()) |line|
        {
            var splits = std.mem.tokenize(u8, line, "|");
            const leftSide = splits.next().?;
            var nums: [10]u64 = std.mem.zeroes([10]u64);

            var numTexts = std.mem.tokenize(u8, leftSide, " ");
            while(numTexts.next()) | numText |
            {
                const indices: u64 = getIndices(numText);

                if(numText.len == 2) nums[1] = indices;
                if(numText.len == 4) nums[4] = indices;
                if(numText.len == 3) nums[7] = indices;
                if(numText.len == 7) nums[8] = indices;
            }

            numTexts = std.mem.tokenize(u8, leftSide, " ");
            while(numTexts.next()) | numText |
            {
                const indices: u64 = getIndices(numText);
                if(numText.len == 6) // numbers 0,6,9 have 6 of the 7 digits set
                {
                    // only 9 has all the same digits set as 4
                    if(checkBInA(indices, nums[4]))
                    {
                        nums[9] = indices;
                    }
                    // both 0 and 9 has same digits set as 7
                    else if(checkBInA(indices, nums[7]))
                    {
                        nums[0] = indices;
                    }
                    else
                    {
                        nums[6] = indices;
                    }
                }
                else if(numText.len == 5) //numbers 2,3,5 have 5 of the 7 digits set
                {
                    // only 3 from the 2,3,5 has same digits as 7 has.
                    if(checkBInA(indices, nums[7]))
                    {
                        nums[3] = indices;
                    }
                    // check if inverted 4, as in bottom, bottomleft and top bit set
                    else if(checkBInA(indices, 127 - nums[4]))
                    {
                        nums[2] = indices;
                    }
                    // if none of the above cases are true, then it must be 5
                    else
                    {
                        nums[5] = indices;
                    }
                }
            }

            const rightSide = splits.next().?;
            numTexts = std.mem.tokenize(u8, rightSide, " ");
            var number: u64 = 0;
            while(numTexts.next()) | numText |
            {
                const indices: u64 = getIndices(numText);
                var i:u32 = 0;
                while(i < 10) : (i += 1)
                {
                    if(nums[i] == indices)
                    {
                        number = number * 10 + i;
                    }
                }
            }
            numberSum += number;

        }

        const res = try std.fmt.bufPrint(printBuffer[printLen..], "Day8-2: 7 digit numbers sum: {}\n", .{numberSum});
        printLen += res.len;
    }
    return printLen;
}

fn checkBInA(a: u64, b: u64) bool
{
    return (a & b) == b;
}

// 7 bits represents the abcdefg being on/off
fn getIndices(numText: []const u8) u64
{
    var indices: u64 = 0;
    var i:u32 = 0;
    while(i < numText.len) : (i += 1)
        indices += @as(u64, 1) << @intCast(u6, numText[i] - 'a');
    return indices;
}