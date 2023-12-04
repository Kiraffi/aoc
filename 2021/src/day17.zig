const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const bytesMax: u32 = 1000;

pub fn day17(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var xTargetStart: i32 = 0;
    var yTargetStart: i32 = 0;

    var xTargetEnd: i32 = 0;
    var yTargetEnd: i32 = 0;

    {
        var lines = std.mem.tokenize(u8, inputFile, "=");
        const theFirst = lines.next().?;
        const first = lines.next().?;
        const second = lines.next().?;
        const firstChar = theFirst[theFirst.len - 1];
        if(firstChar == 'x')
        {
            try parseNumbers(first, &xTargetStart, &xTargetEnd);
            try parseNumbers(second, &yTargetStart, &yTargetEnd);
        }
        else if(firstChar == 'y')
        {
            try parseNumbers(first, &yTargetStart, &yTargetEnd);
            try parseNumbers(second, &xTargetStart, &xTargetEnd);
        }
        if((xTargetStart > 0 and xTargetEnd > 0 and xTargetEnd < xTargetStart) or
            (xTargetStart < 0 and xTargetEnd < 0 and xTargetEnd > xTargetStart))
        {
            swap(&xTargetStart, &xTargetEnd);
        }
        if((yTargetStart > 0 and yTargetEnd > 0 and yTargetEnd < yTargetStart) or
            (yTargetStart < 0 and yTargetEnd < 0 and yTargetEnd > yTargetStart))
        {
            swap(&yTargetStart, &yTargetEnd);
        }
    }

    var resultA: u64 = 0;
    // Assuming ytargetstart and end are always negative!!
    {
        const xDistance = if(xTargetStart > 0) xTargetStart else -xTargetStart;
        const yDistance = if(yTargetEnd > 0) yTargetEnd else -yTargetEnd;

        var powerX: i32 = 1;
        var sumX: i32 = 1;
        while(sumX < xDistance)
        {
            powerX += 1;
            sumX += powerX;
        }

        const powerY: i32 = yDistance - 1;


        // sum function 1+2+3+4+5+6... = n * (n + 1) / 2, arithmetic sum
        // so xpower minimum sum that hits in the area.
        const maxHeight =  @divTrunc((powerY + 1), 2) * powerY;
        resultA = @intCast(u64, maxHeight);
    }
    
    var resultB: u64 = 0;
    {
        const xDistance1 = if(xTargetStart > 0) xTargetStart else -xTargetStart;
        const xDistance2 = if(xTargetEnd > 0) xTargetEnd else -xTargetEnd;
        const yDistance = if(yTargetEnd > 0) yTargetEnd else -yTargetEnd;
        
        var sumX: i32 = 1;
        var xMinPower: i32 = 1;
        var xMaxPower: i32 = 0;
        while(sumX < xDistance1)
        {
            xMinPower += 1;
            sumX += xMinPower;
        }
        xMaxPower = xDistance2;

        var hits: u32 = 0;

        var yPower = -yDistance;
        while(yPower <= yDistance) : (yPower += 1)
        {
            var xPower = xMinPower;
            while(xPower <= xMaxPower) : (xPower += 1)
            {
                var xPos:i32 = 0;
                var yPos:i32 = 0;
                var dX: i32 = xPower;
                var dY: i32 = yPower;
                while(yPos > yTargetEnd and xPos <= xTargetEnd)
                {
                    xPos += dX;
                    dX = if(dX > 0) dX - 1 else 0;
                    yPos += dY;
                    dY -= 1;
                    
                    if(yPos <= yTargetStart and yPos >= yTargetEnd and xPos >= xTargetStart and xPos <= xTargetEnd)
                    {
                        hits += 1;
                        break;
                    }
                }
            }
        }
        resultB = hits;
    }

    const res =try std.fmt.bufPrint(printBuffer, "Day 17-1: Max height: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 17-2: Possible hit combinations: {}\n", .{resultB});
    return res.len + res2.len;
}



fn swap(x1: *i32, x2: *i32) void
{
    const tmp = x1.*;
    x1.* = x2.*;
    x2.* = tmp;
}

fn parseNumbers(str: []const u8, x1: *i32, x2: *i32) anyerror!void
{
    var start: u32 = 0;
    var end: u32 = 0;
    var parsedNumbers: u32 = 0;
    var i: u32 = 0;
    while(i < str.len) : (i += 1)
    {
        const c = str[i];
        if((c >= '0' and c <= '9') or c == '-')
        {
            end = i;
        }
        else
        {
            const value: i32 = try std.fmt.parseInt(i32, str[start..end + 1], 10);
            
            if(parsedNumbers == 0)
            {
                x1.* = value;
                start = i + 2;
                i += 1;
            }
            else if(parsedNumbers == 1)
            {
                x2.* = value;
                return;
            }
            parsedNumbers += 1;
        }
    }
}