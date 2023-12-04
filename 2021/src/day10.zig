const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day10(alloc: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var autoScores = std.ArrayList(u64).init(alloc);
    defer autoScores.deinit();

    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        var errorScore: u64 = 0;
        while (lines.next()) |line|
        {
            var stack: [64]u8 = std.mem.zeroes([64]u8);
            const errorChar: u8 = findErrorChar(line, &stack);
            if(errorChar != 0)
            {
                errorScore += switch(errorChar)
                {
                    '>' => 25137,
                    '}' => 1197,
                    ']' => 57,
                    ')' => 3,
                    else => @as(u64, 0),
                };
            }
            else
            {
                var i:u32 = 0;
                while(stack[i] != 0) : (i += 1) {}
                var score: u64 = 0;
                while(i > 0) : (i -= 1)
                {
                    const val = switch(stack[i - 1])
                    {
                        '<' => 4,
                        '{' => 3,
                        '[' => 2,
                        '(' => 1,
                        else => @as(u64, 0),
                    };
                    score = score * 5 + val;
                }
                try autoScores.append(score);
            }
        }
        const res = try std.fmt.bufPrint(printBuffer, "Day10-1: Syntax error score: {}\n", .{errorScore});
    
        std.sort.sort(u64, autoScores.items, {}, comptime std.sort.asc(u64));
        var i:u64 = (autoScores.items.len) / 2;

        const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day10-2: Score: {}\n", .{autoScores.items[i]});
        return res.len + res2.len;
    }
}


fn findErrorChar(line: []const u8, stack: []u8 ) u8
{
    var stackIndex: u32 = 0;
    var i:u64 = 0;
    var end:u64 = line.len;
    while(i < end) : (i += 1)
    {
        //const ii: u64 = @intCast(u64, i);
        const removeChars: bool = switch(line[i])
        {
            '>' => if(stack[stackIndex - 1] != '<') { return '>'; } else true,
            '}' => if(stack[stackIndex - 1] != '{') { return '}'; } else true,
            ']' => if(stack[stackIndex - 1] != '[') { return ']'; } else true,
            ')' => if(stack[stackIndex - 1] != '(') { return ')'; } else true,
            else => false,
        };
        if(removeChars)
        {
            stackIndex -= 1;
            stack[stackIndex] = 0;
        }
        else
        {
            stack[stackIndex] = line[i];
            stackIndex += 1;
        }
    }
    return 0;
}

