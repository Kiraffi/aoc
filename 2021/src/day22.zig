const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const Box = struct
{
    xi0: i64,
    xi1: i64,
    yi0: i64,
    yi1: i64,
    zi0: i64,
    zi1: i64,
    setState: i64,
    size: i64,
};

pub fn day22(allocator: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;

    var cubeArray = std.ArrayList(Box).init(allocator);
    defer cubeArray.deinit();

    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        while(lines.next()) |line|
        {
            var text = std.mem.tokenize(u8, line, " ");

            const onoff = text.next().?;
            const positions = text.next().?;

            var coords = std.mem.tokenize(u8, positions, ",");

            var froms: [3]i64 = undefined;
            var tos: [3]i64 = undefined;
            var index: u32 = 0;
            while(coords.next()) |coord|
            {
                var fromTo = std.mem.tokenize(u8, coord[2..], "..");
                froms[index] = try std.fmt.parseInt(i64, fromTo.next().?, 10);
                // Add one for including end point
                tos[index] = try std.fmt.parseInt(i64, fromTo.next().?, 10);
                tos[index] += 1;
                index += 1;
            }

            const setState: i64 = if(std.mem.eql(u8, onoff, "on")) 1 else 0;
            const box = Box{
                .xi0 = froms[0], .xi1 = tos[0],
                .yi0 = froms[1], .yi1 = tos[1],
                .zi0 = froms[2], .zi1 = tos[2],
                .setState = setState, .size = (tos[0] - froms[0]) * (tos[1] - froms[1]) * (tos[2] - froms[2])
            };
            try cubeArray.append(box);
        }
    }

    // Part A
    {
        var count: i64 = 0;
        const limitBox = Box{
            // add one for inclusion of 50
            .xi0 = -50, .xi1 = 51,
            .yi0 = -50, .yi1 = 51,
            .zi0 = -50, .zi1 = 51,
            .setState = 0, .size = 101 * 101 * 101
        };
        for(cubeArray.items) |cube, i|
        {
            var limitedBox = getOverlapBox(limitBox, cube);
            limitedBox.setState = cube.setState;
            count += calculateCubesToIndex(cubeArray.items, limitedBox, i);
        }
        resultA = @intCast(u64, count);
    }
    // Part B
    {
        var count: i64 = 0;
        for(cubeArray.items) |cube, i|
        {
            count += calculateCubesToIndex(cubeArray.items, cube, i);
        }
        resultB = @intCast(u64, count);
    }

    const res =try std.fmt.bufPrint(printBuffer, "Day 22-1: Cubes lit: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 22-2: Cubes lit: {}\n", .{resultB});
    return res.len + res2.len;
}

fn calculateCubesToIndex(cubeArray: []Box, currentBox: Box, fromIndex: usize) i64
{
    if(fromIndex >= cubeArray.len)
        return 0;
    if(currentBox.size == 0)
        return 0;
    // If set on, add to count, if off, start with count of 0.
    var count: i64 = currentBox.setState * currentBox.size;
    var i: usize = 0;
    while(i < fromIndex) : (i += 1)
    {
        const cube2 = cubeArray[i];
        var overlap = getOverlapBox(currentBox, cube2);
        overlap.setState = cube2.setState;
        // Remove the amount of cubes with overlap in recursion with
        // previous boxes. Basically if 3 boxes overlap, the first loop iteration
        // adds Box A cube amount, second iteration we add B then remove A and B intersect cubes,
        // third time we add C box, remove C and B intersect, remove C and A intersect and
        // add (C intersect A) and (C intersect B) intersect back.
        count -= calculateCubesToIndex(cubeArray, overlap, i);
    }
    return count;
}

fn getOverlapBox(boxA: Box, boxB: Box) Box
{
    // the size should be at least 0, so no negative boxes.
    var overlapBox: Box = undefined;

    overlapBox.xi0 = @maximum(boxA.xi0, boxB.xi0);
    overlapBox.xi1 = @maximum(overlapBox.xi0, @minimum(boxA.xi1, boxB.xi1));
    overlapBox.yi0 = @maximum(boxA.yi0, boxB.yi0);
    overlapBox.yi1 = @maximum(overlapBox.yi0, @minimum(boxA.yi1, boxB.yi1));
    overlapBox.zi0 = @maximum(boxA.zi0, boxB.zi0);
    overlapBox.zi1 = @maximum(overlapBox.zi0, @minimum(boxA.zi1, boxB.zi1));
    overlapBox.size = (overlapBox.xi1 - overlapBox.xi0) * (overlapBox.yi1 - overlapBox.yi0) * (overlapBox.zi1 - overlapBox.zi0);
    overlapBox.setState = 0;
    return overlapBox;
}