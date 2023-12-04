const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const mapSize: u32 = 100;

pub fn day15(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var map = std.mem.zeroes([mapSize * mapSize]u8);
    var map2 = std.mem.zeroes([mapSize * mapSize * 5 * 5]u8);
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        var y:u32 = 0;
        while (lines.next()) |line|
        {
            var x: u32 = 0;
            while(x < line.len) : (x+=1)
            {
                const v: u8 = line[x] - '0';
                map[y * mapSize + x] = v;

                var i: u32 = 0;
                while(i < 25) : (i += 1)
                {
                    const x1 = i % 5;
                    const y1 = i / 5;
                    var v2 = @intCast(u8, v + x1 + y1);
                    while(v2 > 9) v2 -= 9;
                    const y2 = y + y1 * mapSize;
                    const x2 = x + x1 * mapSize;
                    map2[y2 * mapSize * 5 + x2] = v2;
                }
            }

            y += 1;
        }
    }

    var resultA: u64 = 0;
    var resultB: u64 = 0;

    {
        var travelMap = std.mem.zeroes([mapSize * mapSize]u32);
        fillDistances(&travelMap, &map, 0, 0, mapSize);
        resultA = travelMap[(mapSize - 1) * (1 + mapSize)] - map[0];
    }

    {
        var travelMap = std.mem.zeroes([mapSize * mapSize * 5 * 5]u32);
        fillDistances(&travelMap, &map2, 0, 0, mapSize * 5);
        resultB = travelMap[(mapSize * 5 - 1) * (1 + mapSize * 5)] - map2[0];
    }

    const res =try std.fmt.bufPrint(printBuffer, "Day 15-1: Min distance: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 15-2: Min distance: {}\n", .{resultB});
    return res.len + res2.len;
}

const Point = struct
{
    x: i32,
    y: i32,
};

fn canMove(travelMap: []u32, map: []const u8, p: Point, dir: Point, distance: u64, newMapSize: u32) bool
{
    if(p.x + dir.x < 0 or p.x + dir.x > newMapSize - 1 or p.y + dir.y < 0 or p.y + dir.y > newMapSize - 1)
        return false;

    const index = getIndex(p, dir, newMapSize);
    const newDistance: u64 = distance + map[index];
    return (newDistance < travelMap[index] or travelMap[index] == 0);
}
fn getIndex(p: Point, dir: Point, newMapSize: u32) u32
{
    return @intCast(u32, p.x + dir.x + (p.y + dir.y) * @intCast(i32, newMapSize));
}

fn fillDistances(travelMap: []u32, map: []const u8, startX: u32, startY: u32, newMapSize: u32) void
{

    const maxSize: u32 = @as(u32, 1) << 16;

    var moves: u32 = 1;
    var movementStack = std.mem.zeroes([2][maxSize]u64);
    movementStack[0][0] = startX + startY * newMapSize;
    const dirs: [4]Point = .{
        Point{.x = -1, .y = 0 },
        Point{.x = 1, .y = 0 },
        Point{.x = 0, .y = -1 },
        Point{.x = 0, .y = 1 },
    };
    var stackIndex: u32 = 0;
    while(moves > 0)
    {
        var newStackIndex: u32 = (stackIndex + 1) % 2;
        var oldMoves = moves;
        moves = 0;
        var i: u32 = 0;
        while(i < oldMoves) : (i += 1)
        {
            const movementStackIndex = movementStack[stackIndex][i];
            const index = @intCast(u32, movementStackIndex  & 0xffff_ffff);
            const p: Point = Point {
                .x = @intCast(i32, index % newMapSize),
                .y = @intCast(i32, index / newMapSize),
            };

            const newDistance: u64 = (movementStackIndex >> 32) + @intCast(u64, map[index]);
            if(newDistance > travelMap[index] and travelMap[index] > 0)
                continue;
            travelMap[index] = @intCast(u32, newDistance);

            const addedIndex: u64 = (newDistance & 0xffff_ffff) << 32;
            var dirLoop: u32 = 0;
            while(dirLoop < 4) : (dirLoop += 1)
            {
                if(canMove(travelMap, map, p, dirs[dirLoop], newDistance, newMapSize))
                {
                    const newIndex = getIndex(p, dirs[dirLoop], newMapSize);
                    travelMap[newIndex] = @intCast(u32, newDistance) + map[newIndex];
                    movementStack[newStackIndex][moves] = newIndex + addedIndex;
                    moves += 1;
                }
            }
        }

        stackIndex = (stackIndex + 1) % 2;
    }
}