const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


//pub fn day7(alloc: *std.mem.Allocator, comptime inputFile: []const u8 ) anyerror!void
pub fn day7(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    //var lines = std.mem.tokenize(u8, inputFile, "\r\n");
    //var numbersIter = std.mem.tokenize(u8, lines.next().?, ",");

    var spawns: [2000]u64 = std.mem.zeroes([2000]u64);
    var maxNumber: u32 = 0;
    {
        var charIndex: u32 = 0;
        var num:u64 = 0;
        var pars: bool = false;
        while (charIndex < inputFile.len) : (charIndex += 1)
        {
            const c = inputFile[charIndex];
            if(c >= '0' and c <= '9')
            {
                pars = true;
                num = num * 10 + @intCast(u64, c - '0');
            }
            else if(pars)
            {
            //const num = try std.fmt.parseInt(u64, numberString, 10);
                spawns[num] += 1;
                maxNumber = @maximum(maxNumber, @intCast(u32, num) + 1);
                pars = false;
                num = 0;
            }
        }
    }
    var printLen: usize = 0;
    {
        var lowFuel:u64 = 0;
        var lowIndex:u32 = 0;
        var lowSumAtIndex:u64 = spawns[lowIndex];

        var highFuel:u64 = 0;
        var highIndex:u32 = maxNumber;
        var highSumAtIndex:u64 = spawns[highIndex];

        while(lowIndex < highIndex)
        {
            if(lowSumAtIndex <= highSumAtIndex)
            {
                lowFuel += lowSumAtIndex;
                lowIndex += 1;
                lowSumAtIndex += spawns[lowIndex];
            }
            else
            {
                highFuel += highSumAtIndex;
                highIndex -= 1;
                highSumAtIndex += spawns[highIndex];
            }
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day7-1: Index: {}, fuel consumption: {}\n", .{lowIndex, lowFuel + highFuel});
        printLen = res.len;
    }

    {
        var i:u32 = 0;
        var j:u32 = maxNumber - 1;
        var valueAtI: u64 = calculateToDistance(&spawns, 0, maxNumber);
        var valueAtJ: u64 = calculateToDistance(&spawns, j, maxNumber);
        var lowestFuel:u64 = @minimum(valueAtI, valueAtJ);
        while(i < maxNumber)
        {
            var val: u64 = calculateToDistance(&spawns, (i + j) / 2, maxNumber);
            lowestFuel = @minimum(lowestFuel, val);
            if(val < valueAtI)
            {
                if(valueAtI < valueAtJ)
                {
                    valueAtJ = val;
                    j = (i + j) / 2;
                }
                else
                {
                    valueAtI = val;
                    i = (i + j) / 2;
                }
            }
            else if(val < valueAtJ)
            {
                valueAtJ = val;
                j = (i + j) / 2;
            }
            if(i + 1 >= j)
                break;
        }
        const res = try std.fmt.bufPrint(printBuffer[printLen..], "Day7-2: Fuel consumption: {}\n", .{lowestFuel});
        return printLen + res.len;
    }
}

fn calculateToDistance(numbers: []const u64, target: u32, iterations: u32) u64
{
    var result:u64 = 0;
    var i:u32 = 0;
    while(i < iterations) : (i += 1)
    {
        const d:u64 = getDistance(i, target);
        result += numbers[i] * d * (d + 1) / 2; // arithmetic sum
    }
    return result;
}

fn getDistance(a: u32, b: u32) u64
{
    return if(a < b) b - a else a - b;
}