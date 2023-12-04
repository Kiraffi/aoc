const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const MapWidth:usize = 13;
const MapHeight:usize = 7;

var lowestMoveCost: u64 = ~@as(u64, 0);

const Coord = struct
{
    // could pack xy into single u8
    x: u8,
    y: u8,
};

const Position = struct
{
    cost: u64,
    abcd: [4 * 4]Coord,
};

var allStates: *std.ArrayList(u64) = undefined;
var allCosts: *std.ArrayList(u32) = undefined;
var states: *std.ArrayList(u64) = undefined;
var costs: *std.ArrayList(u32) = undefined;

var allStateLookup: *std.AutoHashMap(u64, u32) = undefined;
var stateLookup: *std.AutoHashMap(u64, u32) = undefined;

const Costs: [4]u32 = .{1, 10, 100, 1000};
var map: [MapWidth * MapHeight]u8 = undefined;

pub fn day23(alloc: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;

    std.mem.set(u8, &map, ' ');

    // Part A
    {
        var startingState = parse(inputFile, 2);

        states = &std.ArrayList(u64).init(alloc);
        defer states.deinit();

        allStates = &std.ArrayList(u64).init(alloc);
        defer allStates.deinit();

        allCosts = &std.ArrayList(u32).init(alloc);
        defer allCosts.deinit();

        costs = &std.ArrayList(u32).init(alloc);
        defer costs.deinit();

        allStateLookup = &std.AutoHashMap(u64, u32).init(alloc);
        defer allStateLookup.deinit();

        stateLookup = &std.AutoHashMap(u64, u32).init(alloc);
        defer stateLookup.deinit();

        try allStates.append(startingState);
        try states.append(startingState);
        try allCosts.append(0);
        try costs.append(0);
        try allStateLookup.put(startingState, 0);
        try stateLookup.put(startingState, 0);

        while(resultA == 0)
        {
            const cheapestIndex = findCheapestStateIndex();
            const cheapestState = states.items[cheapestIndex];
            const cheapestStateCost = costs.items[cheapestIndex];

            states.items[cheapestIndex] = states.items[states.items.len - 1];
            _ = states.pop();

            costs.items[cheapestIndex] = costs.items[costs.items.len - 1];
            _ = try stateLookup.put(states.items[cheapestIndex], cheapestIndex);
            _ = costs.pop();
            _ = stateLookup.remove(cheapestState);

            resultA = try doCheapestMove(cheapestState, cheapestStateCost, 2);
            if(costs.items.len == 0)
            {
                break;
            }
        }
    }
    // Part B
    {
        var newInputFile: [256]u8 = undefined;
        std.mem.set(u8, &newInputFile, ' ');
        newInputFile[255] = 0;
        var inputSize: usize = 0;
        {
            var lines = std.mem.tokenize(u8, inputFile, "\r\n");
            var lineIndex: usize = 0;
            while(lines.next()) |line|
            {
                std.mem.copy(u8, newInputFile[inputSize..], line);
                inputSize += line.len;
                newInputFile[inputSize + 0] = '\r';
                newInputFile[inputSize + 1] = '\n';
                inputSize += 2;
                lineIndex += 1;
                if(lineIndex == 3)
                {
                    const line1 = "  #D#C#B#A#\r\n";
                    const line2 = "  #D#B#A#C#\r\n";
                    std.mem.copy(u8, newInputFile[inputSize..], line1);
                    inputSize += line1.len;
                    std.mem.copy(u8, newInputFile[inputSize..], line2);
                    inputSize += line2.len;
                    lineIndex += 2;
                }
            }
        }
        var startingState = parse(newInputFile[0..inputSize], 4);
        //printMap(startingState, 4);

        states = &std.ArrayList(u64).init(alloc);
        defer states.deinit();

        allStates = &std.ArrayList(u64).init(alloc);
        defer allStates.deinit();

        allCosts = &std.ArrayList(u32).init(alloc);
        defer allCosts.deinit();

        costs = &std.ArrayList(u32).init(alloc);
        defer costs.deinit();

        allStateLookup = &std.AutoHashMap(u64, u32).init(alloc);
        defer allStateLookup.deinit();

        stateLookup = &std.AutoHashMap(u64, u32).init(alloc);
        defer stateLookup.deinit();


        try states.append(startingState);
        try allStates.append(startingState);
        try allCosts.append(0);
        try costs.append(0);
        try allStateLookup.put(startingState, 0);
        try stateLookup.put(startingState, 0);
        while(resultB == 0)
        {
            const cheapestIndex = findCheapestStateIndex();
            const cheapestState = states.items[cheapestIndex];
            const cheapestStateCost = costs.items[cheapestIndex];

            states.items[cheapestIndex] = states.items[states.items.len - 1];
            _ = states.pop();

            costs.items[cheapestIndex] = costs.items[costs.items.len - 1];
            _ = try stateLookup.put(states.items[cheapestIndex], cheapestIndex);
            _ = costs.pop();
            _ = stateLookup.remove(cheapestState);

            resultB = try doCheapestMove(cheapestState, cheapestStateCost, 4);
            if(resultB != 0 or costs.items.len == 0)
                break;
        }
    }



    const res =try std.fmt.bufPrint(printBuffer, "Day 23-1: Cheapest: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 23-2: Cheapest: {}\n", .{resultB});
    return res.len + res2.len;
}

fn parse(input: []const u8, amphibodsLen: u8) u64
{
    var startingState: u64 = 0;
    // Parse lines to strings....
    var lines = std.mem.tokenize(u8, input, "\r\n");
    var lineIndex: u8 = 0;

    var positions: [4 * 4]Coord = undefined;
    var counts = std.mem.zeroes([4]u8);


    while(lines.next()) |line|
    {
        for(line) |c, i|
        {
            const mapIndex = i + lineIndex * MapWidth;
            if(c >= 'A' and c <= 'D')
            {
                const index: u64 = c - 'A';

                positions[index * amphibodsLen + counts[index]] =
                    Coord {.x = @intCast(u8, i), .y = lineIndex };
                counts[index] += 1;

                map[mapIndex] = '.';
            }
            else
            {
                map[mapIndex] = c;
            }
        }
        lineIndex += 1;
    }

    startingState = packState(positions[0..amphibodsLen * 4]);
    //printMap(startingState, amphibodsLen);
    return startingState;
}

fn findCheapestStateIndex() u32
{
    var cheapestIndex: u32 = 0;
    var cheapestCost: u32 = costs.items[0];
    for(costs.items) |cost, i|
    {
        if(cost < cheapestCost)
        {
            cheapestCost = cost;
            cheapestIndex = @intCast(u32, i);
        }
    }
    return cheapestIndex;
}

fn doCheapestMove(cheapestState: u64, cheapestCost: u32, amphibodsLen: u8) anyerror!u64
{
    var canMoves: [4 * 4]bool = .{
        true, true, true, true,
        true, true, true, true,
        true, true, true, true,
        true, true, true, true,
    };

    const goalY = amphibodsLen + 1;

    var goals: [4]Coord = .{
        Coord{.x = 3, .y = goalY},
        Coord{.x = 5, .y = goalY},
        Coord{.x = 7, .y = goalY},
        Coord{.x = 9, .y = goalY},
    };

    var positions: [4 * 4]Coord = undefined;
    var counts = std.mem.zeroes([4]u8);
    unpackState(cheapestState, amphibodsLen, &positions, &counts);

    var matchingPositions: usize = 0;
    var i: u8 = 0;
    while(i < 4) : (i += 1)
    {
        var updatedGoal = true;
        while(updatedGoal)
        {
            updatedGoal = false;
            var j: usize = amphibodsLen;
            while(j > 0) : (j -= 1)
            {
                const index = j - 1 + i * amphibodsLen;

                if(!canMoves[index])
                    continue;
                if(checkPosition(positions[index], goals[i]))
                {
                    if(goals[i].y > 2)
                        goals[i].y -= 1;
                    canMoves[index] = false;
                    matchingPositions += 1;
                    updatedGoal = true;
                }
            }
        }

        var j: usize = 0;
        while(j < amphibodsLen) : (j += 1)
        {
            const index = j + i * amphibodsLen;
            if(!canMoves[index])
                continue;
            const startPos = positions[index];

            // If anyone can move to goal, that should be the only move possible
            // from current state.
            const moves = canMoveTo(cheapestState, startPos, goals[i]);
            if(moves > 0)
            {
                positions[index] = goals[i];
                const newState = packState(positions[0..amphibodsLen * 4]);
                const newCost = cheapestCost + moves * Costs[i];
                _ = try addToList(newState, newCost);
                return 0;
            }
        }
    }
    i = 0;
    while(i < 4 * amphibodsLen) : (i += 1)
    {
        if(!canMoves[i] or positions[i].y == 1)
            continue;
        var targets: [16]Coord = undefined;
        const startPos = positions[i];
        var targetCount: u8 = 0;

        if(startPos.x > 1)
        {
            var target = Coord{.x = startPos.x - 1, .y = 1};
            while(target.x >= 1)
            {
                if(target.x == 3 or target.x == 5 or target.x == 7 or target.x == 9)
                    target.x -= 1;
                if((cheapestState >> @intCast(u6, target.x - 1)) & 1 != 0)
                    break;
                targets[targetCount] = target;
                targetCount += 1;
                target.x -= 1;
            }
        }
        if(startPos.x < MapWidth - 2)
        {
            var target = Coord{.x = startPos.x + 1, .y = 1};
            while(target.x <= MapWidth - 2)
            {
                if(target.x == 3 or target.x == 5 or target.x == 7 or target.x == 9)
                    target.x += 1;
                if((cheapestState >> @intCast(u6, target.x - 1)) & 1 != 0)
                    break;

                targets[targetCount] = target;
                targetCount += 1;
                target.x += 1;
            }
        }

        var j: u8 = 0;
        while(j < targetCount) : (j += 1)
        {
            const moves = canMoveTo(cheapestState, startPos, targets[j]);
            if(moves > 0)
            {
                var newPositions = positions;
                newPositions[i] = targets[j];
                const newState = packState(newPositions[0..amphibodsLen * 4]);
                const newCost = cheapestCost + moves * Costs[i  / amphibodsLen];
                _ = try addToList(newState, newCost);
            }

        }
    }
    if(matchingPositions == 4 * amphibodsLen)
    {
        //printMap(cheapestState, amphibodsLen);
        return cheapestCost;
    }
    return 0;
}

fn packState(positions: []Coord) u64
{
    var result: u64 = 0;
    var count: u6 = 0;
    const amphibodsLen: u6 = @intCast(u6, positions.len / 4);
    var x: u6 = 1;
    while(x < 16) : (x += 1)
    {
        const coord = Coord{.x = x, .y = 1};
        for(positions) |pos, i|
        {
            if(checkPosition(pos, coord))
            {
                result |= @as(u64, 1) << (x - 1);
                result |= @intCast(u64, i / amphibodsLen) << (32 + count * 2);
                count += 1;
                break;
            }
        }
    }

    x = 0;
    while(x < 4) : (x += 1)
    {
        var y: u6 = 0;
        while(y < amphibodsLen) : (y += 1)
        {
            const coord = Coord{.x = x * 2 + 3, .y = y + 2};
            const spotIndex: u6 = 16 + y + x * 4;
            for(positions) |pos, i|
            {
                if(checkPosition(pos, coord))
                {
                    result |= @as(u64, 1) << spotIndex;
                    result |= @intCast(u64, i / amphibodsLen) << (32 + count * 2);
                    count += 1;
                    break;
                }
            }

        }
    }
    //printMap(result, amphibodsLen);
    return result;
}

fn unpackState(state: u64, amphibodsLen: u8, positions: []Coord, counts: []u8) void
{
    var spotIndex: u6 = 0;
    var charIndex: u6 = 0;
    while(spotIndex < 32) : (spotIndex += 1)
    {
        if((state >> spotIndex) & 1 != 0)
        {
            const ch = @intCast(u8, (state >> (32 + charIndex * 2)) & 3);
            const x: u8 = if(spotIndex < 16) spotIndex + 1
                else 3 + ((spotIndex - 16) / 4) * 2;
            const y: u8 = if(spotIndex < 16) 1 else ((spotIndex - 16) % 4) + 2;
            positions[counts[ch] + amphibodsLen * ch] = Coord{.x = x, .y = y};
            counts[ch] += 1;
            charIndex += 1;
        }
    }
}


fn printMap(state: u64, amphibodsLen: u8) void
{
    var positions: [4 * 4]Coord = undefined;
    var counts = std.mem.zeroes([4]u8);
    unpackState(state, amphibodsLen, &positions, &counts);

    var y: u6 = 0;
    while(y < MapHeight) : (y += 1)
    {
        var x: u6 = 0;
        while(x < MapWidth) : (x += 1)
        {
            var ch: u8 = map[x + y * MapWidth];
            if(ch == '.')
            {
                for(positions) |pos, i|
                {
                    if(checkPosition(pos, Coord{.x = x, .y = y}))
                    {
                        ch = 'A' + @intCast(u8, i / amphibodsLen);
                    }
                }
            }

            print("{c}", .{ch});
        }
        print("\n", .{});
    }
}


fn canMoveTo(state: u64, startPos: Coord, endPos: Coord) u32
{
    var checkState: u64 = 0;

    // the bits are 16 bits up row, 4 bits A row, 4 bits B row, 4 bits C row, 4 bits D row.
    const addLen: u8 = if(startPos.y > 1) 1 else 0;
    if(startPos.y > 1)
    {
        // set movement bits.
        // notice we do not add the start pos, but tile next from it, and y starts from 2.
        const line = (@as(u64, 1) << @intCast(u6, startPos.y - 2)) - 1;
        checkState |= line << @intCast(u6, ((startPos.x - 3) / 2) * 4 + 16);

    }

    if(endPos.y > 1)
    {
        // notice we add the endpos too, and y starts from 2.
        const line = (@as(u64, 1) << @intCast(u6, endPos.y - 1)) - 1;
        checkState |= line << @intCast(u6, ((endPos.x - 3) / 2) * 4 + 16);
    }
    if(startPos.x < endPos.x)
    {
        const line = (@as(u64, 1) << @intCast(u6, endPos.x - startPos.x + addLen)) - 1;
        checkState |= line << @intCast(u6, startPos.x - addLen);
    }
    else if(startPos.x > endPos.x)
    {
        const line = (@as(u64, 1) << @intCast(u6, startPos.x - endPos.x + addLen)) - 1;
        checkState |= line << @intCast(u6, endPos.x - 1);
    }

    //printState(checkState);
    if(checkState & state != 0)
        return 0;

    return @popCount(u64, checkState);
}

fn printState(state: u64) void
{
    var y: u6 = 0;
    while(y < MapHeight) : (y += 1)
    {
        var x: u6 = 0;
        while(x < MapWidth) : (x += 1)
        {
            var ch: u8 = map[x + y * MapWidth];
            if(ch == '.')
            {
                var spotIndex: u6 = if(y == 1) x - 1 else 16 + ((x - 3) / 2) * 4 + (y - 2);
                if((state >> spotIndex) & 1 != 0)
                {
                    ch = 'O';
                }
            }

            print("{c}", .{ch});
        }
        print("\n", .{});
    }
}


fn addToList(state: u64, cost: u32) anyerror!bool
{
    const old = allStateLookup.get(state);
    if(old != null)
    {
        const ind = old.?;
        if(cost >= allCosts.items[ind])
            return false;
        allCosts.items[ind] = cost;

        const old2 = stateLookup.get(state);
        if(old2 != null)
        {
            costs.items[old2.?] = cost;
            return true;
        }

        try stateLookup.put(state, @intCast(u32, states.items.len));
        try states.append(state);
        try costs.append(cost);
        return true;
    }
    try stateLookup.put(state, @intCast(u32, states.items.len));
    try allStateLookup.put(state, @intCast(u32, allStates.items.len));
    try allStates.append(state);
    try allCosts.append(cost);
    try states.append(state);
    try costs.append(cost);
    return true;
}


fn isFree(pos: Position, coord: Coord) bool
{
    for(pos.abcd) |abcd|
    {
        if(checkPosition(abcd, coord))
            return false;
    }

    return true;
}
fn checkPosition(a: Coord, b: Coord) bool
{
    return a.x == b.x and a.y == b.y;
}