const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

pub fn day25(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;
    var map: [150 * 150]u8 = undefined;
    var mapHeight: u32 = 0;
    var mapWidth: u32 = 0;
    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");

        while(lines.next()) |line|
        {
            mapWidth = @intCast(u32, line.len);
            std.mem.copy(u8, map[mapHeight * line.len..], line);
            mapHeight += 1;
        }
    }

    // Part A
    {
        //printMap(&map, mapWidth, mapHeight);
        //print("\n", .{});

        var moved = true;
        var moves: u32 = 0;
        while(moved)
        {
            moved = false;
            var y: u32 = 0;
            while(y < mapHeight) : (y += 1)
            {
                var x: u32 = 0;
                const lastCanMove = map[(y + 1) * mapWidth - 1] == '>' and map[y * mapWidth] == '.';
                while(x < mapWidth - 1) : (x += 1)
                {
                    const xi = (x + 1) % mapWidth;
                    if(map[y * mapWidth + x] == '>' and map[y * mapWidth + xi] == '.')
                    {
                        map[y * mapWidth + x] = '.';
                        map[y * mapWidth + xi] = '>';
                        moved = true;
                        x += 1;
                    }
                }
                if(lastCanMove)
                {
                        map[y * mapWidth + mapWidth - 1] = '.';
                        map[y * mapWidth] = '>';
                }
            }
            var x: u32 = 0;
            while(x < mapWidth) : (x += 1)
            {
                y = 0;
                const lastCanMove = map[(mapHeight - 1) * mapWidth + x] == 'v' and map[x] == '.';
                while(y < mapHeight - 1) : (y += 1)
                {
                    const yi = (y + 1) % mapHeight;
                    if(map[y * mapWidth + x] == 'v' and map[yi * mapWidth + x] == '.')
                    {
                        map[y * mapWidth + x] = '.';
                        map[yi * mapWidth + x] = 'v';
                        moved = true;
                        y += 1;
                    }
                }
                if(lastCanMove)
                {
                    map[(mapHeight - 1) * mapWidth + x] = '.';
                    map[x] = 'v';
                }
            }
            moves += 1;
            //printMap(&map, mapWidth, mapHeight);
            //print("{}\n\n", .{moves});
        }
        resultA = moves;
    }

    const res =try std.fmt.bufPrint(printBuffer, "Day 25-1: Moves: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 25-2: {}\n", .{resultB});
    return res.len + res2.len;
}

fn printMap(map: []u8, mapWidth: u32, mapHeight: u32) void
{
    var y: u32 = 0;
    while(y < mapHeight) : (y += 1)
    {
        var x: u32 = 0;
        while(x < mapWidth) : (x += 1)
        {
            print("{c}", .{map[y * mapWidth + x]});
        }
        print("\n", .{});
    }
}