const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

pub fn day3(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var strLen = try day3_1(inputFile, printBuffer);
    strLen += try day3_2(inputFile, printBuffer, strLen);
    return strLen;
}

pub fn day3_1(inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var lines = std.mem.tokenize(u8, inputFile, "\r\n");

    var numbers: [16]i32 = std.mem.zeroes([16]i32);
    var numberCount: u32 = 0;
    while (lines.next()) |line|
    {
        numberCount = 0;
        var i:usize = 0;
        while (i < line.len) : (i += 1)
        {
            if(line[i] == '1')
            {
                numbers[i] += 1;
                numberCount += 1;
            }
            else if(line[i] == '0')
            {
                numbers[i] -= 1;
                numberCount += 1;
            }
        }
    }
    {
        // using zeroes so no need to xor and mask bits
        var ones:u32 = 0;
        var zeros: u32 = 0;
        var i:u32 = 0;
        while( i < numberCount ) : (i += 1)
        {
            ones = ones << 1;
            zeros = zeros << 1;
            if(numbers[i] >0)
            {
                ones += 1;
            }
            else
            {
                zeros += 1;
            }
        }

        const res = try std.fmt.bufPrint(printBuffer, "Day3-1: Consumption {d}\n", .{ones * zeros});
        return res.len;
    }
}

pub fn day3_2(inputFile: []const u8, printBuffer: []u8, printLen: usize) anyerror! usize
{
    var lines = std.mem.tokenize(u8, inputFile, "\r\n");

    // count how many partial sequences are there like numbers starting with 101,
    // reserve each number its own counter
    var numbers: [1<<16]u16 = std.mem.zeroes([1<<16]u16);
    while (lines.next()) |line|
    {
        var i:usize = 0;
        // Add extra bit so that we can make sure
        // that 000 and 00 will be pointing different indexes
        var memoryIndex: u32 = 2;
        while (i < line.len) : (i += 1)
        {
            if(line[i] == '1')
            {
                numbers[memoryIndex + 1] += 1;
                memoryIndex = (memoryIndex + 1) << 1;
            }
            else if(line[i] == '0')
            {
                numbers[memoryIndex] += 1;
                memoryIndex = (memoryIndex << 1);
            }
        }
    }
    // Add extra bit so that we can make sure
    // that 000 and 00 will be pointing different indexes
    var memoryIndexOnes: u32 = 2;
    var memoryIndexZeros: u32 = 2;

    {
        var i:u32 = 0;
        while( i < 14 ) : (i += 1)
        {
            const z10 = numbers[memoryIndexOnes];
            const z11 = numbers[memoryIndexOnes + 1];
            if(z11 >= z10 and z11 > 0)
            {
                memoryIndexOnes = (memoryIndexOnes + 1) << 1;

            }
            else if(z10 > 0)
            {
                memoryIndexOnes = (memoryIndexOnes << 1);
            }

            const z00 = numbers[memoryIndexZeros];
            const z01 = numbers[memoryIndexZeros + 1];
            if( (z00 <= z01 or z01 == 0) and z00 > 0)
            {
                memoryIndexZeros = (memoryIndexZeros << 1);
            }
            else if(z01 > 0)
            {
                memoryIndexZeros = (memoryIndexZeros + 1) << 1;
            }
        }
        memoryIndexOnes >>= 1;
        memoryIndexZeros >>= 1;
    }
    // Remove most significant bit....... u5s????
    {
        var zeroHighestBitSet: u5 = 1;
        var oneHighestBitSet: u5 = 1;
        var i:u5 = 0;
        while(i < 31) : (i += 1)
        {
            if(((memoryIndexZeros >> i) & 1) == 1)
            {
                zeroHighestBitSet = i;
            }
            if(((memoryIndexOnes >> i) & 1) == 1)
            {
                oneHighestBitSet = i;
            }
        }
        memoryIndexZeros &= (@as(u32, 1) << @as(u5, zeroHighestBitSet)) - 1;
        memoryIndexOnes &= (@as(u32, 1) << @as(u5, oneHighestBitSet)) - 1;
    }

    const res = try std.fmt.bufPrint(printBuffer[printLen..], "Day3-2: Life support rating: {d}\n", .{memoryIndexOnes * memoryIndexZeros});
    return res.len;
}