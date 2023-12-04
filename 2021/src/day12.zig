const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

// helper for building connections.
const Connections = struct {
    conns: [16]u32,
};

const StartChar: u8 = 30;
const EndChar: u8 = 31;

var connections: [32]Connections = undefined;

// for prefix sum connections.
var connCounts: [32]u8 = undefined;
var connStartIndex: [32]u8 = undefined;
var allConns: [64]u32 = undefined;

//var minimumPaths: [128]u32 = undefined;
//var minimumPathStartindex: [32]u8 = undefined;
//var minimumPathCounts: [32]u8 = undefined;

fn addConnection(fromIndex: u8, toIndex: u8) void
{
    var connCount: u8 = connCounts[fromIndex];
    var con: *Connections = &connections[fromIndex];
    var i:u8 = 0;
    while(i < connCount) :(i += 1)
    {
        if(con.conns[i] == toIndex)
            return;
    }
    if(connCount >= 16)
    {
        print("Failed to add connection, too many connections: {} at index: {} \n", .{connCount, fromIndex});
        return;
    }
    con.conns[connCount] = toIndex;
    connCounts[fromIndex] += 1;
}

fn getIndexFromChar(remapTable: [][2]u8, str: []const u8) u8
{
    if(std.mem.eql(u8, str, "start"))
        return StartChar
    else if(std.mem.eql(u8, str, "end"))
        return EndChar
    else if(str.len != 2)
        return 0;
    var result: u8 = 0;
    var i: u8 = if(str[0] >= 'a' and str[0] <= 'z') 0 else 16;
    while(i < 32) : (i += 1)
    {
        const cmpTable: []const u8 = &remapTable[i];
        if(std.mem.eql(u8, cmpTable, str))
        {
            return i;
        }
        else if(std.mem.eql(u8, cmpTable, &std.mem.zeroes([2]u8)))
        {
            //print("Remapped: {s} to {}\n", .{str, i});
            std.mem.copy(u8, &remapTable[i], str);
            return i;
        }
    }
    return result;
}

pub fn day12(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    connections = std.mem.zeroes([32]Connections);
    connStartIndex = std.mem.zeroes([32]u8);
    connCounts = std.mem.zeroes([32]u8);
    allConns = std.mem.zeroes([64]u32);

    {
        var remapTable = std.mem.zeroes([32][2]u8);

        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        while (lines.next()) |line|
        {
            var sidesIter = std.mem.tokenize(u8, line, "-");
            const left = sidesIter.next().?;
            const right = sidesIter.next().?;
            const lVal: u8 = getIndexFromChar(&remapTable, left);
            const rVal: u8 = getIndexFromChar(&remapTable, right);
            if(rVal != StartChar)
                addConnection(lVal, rVal);
            if(lVal != StartChar)
                addConnection(rVal, lVal);
        }
    }
    {
        // Merge connections from big rooms to others, and remove them.
        // Adding more connections from cave to cave, and if founding
        // multiple connections, just add multiplier for permutations.
        var i: u32 = 0;
        while(i < 32) : (i += 1)
        {
            if(i >= 16 and i != StartChar)
                continue;
            var j:u32 = 0;
            while(j < connCounts[i]) : (j+=1)
            {
                const connLeft = connections[i].conns[j] & 31;
                // Find connections that are into big room.
                if(connLeft < 16 or connLeft == EndChar)
                    continue;

                var k: u32 = 0;
                // Add every connection from big room to cave room!
                while(k < connCounts[connLeft]) : (k += 1)
                {
                    const connRight = connections[connLeft].conns[k];

                    // check if already added.
                    const lIndex = std.mem.indexOfScalar(u32,
                        connections[i].conns[0..connCounts[i]], connRight);
                    if(lIndex == null)
                    {
                        connections[i].conns[connCounts[i]] = connRight;
                        connCounts[i] += 1;
                    }
                    else
                    {
                        const lIndexValue = lIndex.?;
                        connections[i].conns[lIndexValue] += @as(u32, 1) << @intCast(u5, 5);
                    }
                }
            }
       }





    }
    {
        // Do prefix sum of connections, and
        // lay connections in array struct to increase
        // cache locality
        var i: u32 = 0;
        var ind: u8 = 0;
        while(i < 32) : (i += 1)
        {
            connStartIndex[i] = ind;
            var j: u32 = 0;
            while(j < connCounts[i]) : (j += 1)
            {
                var c: u32 = connections[i].conns[j];
                const index = c & 31;
                if(!(index == EndChar or index < 16))
                    continue;
                if(index < 16)
                    c |= @as(u32, 1) << @intCast(u5, 8 + index);
                allConns[ind] = c;
                ind += 1;
            }
            connCounts[i] = ind - connStartIndex[i];
        }


        //// Seems this is slower to add cheking of the paths and building
        //// the graph, rather than just doing the checks.
        ////
        //// Build min needed paths
        //{
        //    std.mem.set(u32, &minimumPaths, 0xffff_ffff);
        //    var sumOfPaths: u8 = 0;
        //    i = 0;
        //    while(i < 32) : (i += 1)
        //    {
        //        minimumPathStartindex[i] = sumOfPaths;
        //        minimumPathCounts[i] = 0;
        //        if(connCounts[i] == 0)
        //            continue;
        //        var pathCount: u32 = 0;
        //        pathCount = buildMinimumPaths(EndChar, i, 0, minimumPaths[sumOfPaths..], pathCount);
        //
        //        minimumPathCounts[i] = @intCast(u8, pathCount);
        //        sumOfPaths += minimumPathCounts[i];
        //    }
        //
        //}
    }
    var resultA = checkConnection(@as(u32, 0), StartChar, 1);
    var resultB = checkConnection(@as(u32, 0), StartChar, 2);

    const res = try std.fmt.bufPrint(printBuffer, "Day12-1: Routes: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day12-2: Routes: {}\n", .{resultB});
    return res.len + res2.len;
}

// Makes slower to try to find always ending paths and comparing those
// than to just travel whole graph even if sometimes doing extra steps.
//fn buildMinimumPaths(index: u32, searchIndex: u32, visited: u32, paths: []u32, pathAmount: u32) u32
//{
//    const ind = @as(u32, 1) << @intCast(u5, index & 31);
//    if(visited & ind != 0)
//        return pathAmount;
//    var newPathAmount = pathAmount;
//
//    if(index & 31 == searchIndex)
//    {
//        var newPath: u32 = visited & 0xffff;
//        var i:u32 = 1;
//        // avoid overflow when removing
//        while(i <= newPathAmount) : (i += 1)
//        {
//            const p = paths[i - 1];
//            const newPandP = newPath & p;
//            // if smaller or equal already exists,
//            // dont do anything
//            if(newPandP == p)
//            {
//                return newPathAmount;
//            }
//            // if the path doesnt have less but still has
//            // atleast same bits set, it must have more,
//            // so remove it from the list.
//            else if(newPandP == newPath)
//            {
//                paths[i - 1] = paths[newPathAmount - 1];
//                newPathAmount -= 1;
//                i -= 1;
//            }
//        }
//        paths[newPathAmount] = newPath;
//        newPathAmount += 1;
//        return newPathAmount;
//    }
//
//    var i:u8 = connStartIndex[index & 31];
//    const connCount = i + connCounts[index & 31];
//
//    const newVisited = visited | ind;
//
//    while(i < connCount) : (i += 1)
//    {
//        newPathAmount = buildMinimumPaths(allConns[i] & 31, searchIndex, newVisited, paths, newPathAmount);
//    }
//    return newPathAmount;
//}


fn checkConnection(visited: u32, index: u32, maxVisits: u8) u32
{
    if(index == EndChar)
        return 1;

    const ind = (index & 15);
    const ind1 = @intCast(u5, ind);

    const thisVisited = (index >> 8);
    const newVisited = visited | thisVisited;

    const visitAmount = (((thisVisited & visited) >> ind1) & 3);
    if(visitAmount >= maxVisits)
        return 0;

    const newMaxVisit = maxVisits - @intCast(u8, visitAmount);

    // Trying to check if the path always reaches exit seems slower
    // than just going as far as can.
    //if((index & 31) < 16)
    //{
    //    var i: u32 = minimumPathStartindex[index & 31];
    //    const loopEnd: u32 = minimumPathCounts[index & 31] + i;
    //    var canExit = false;
    //    while(i < loopEnd) : (i += 1)
    //    {
    //        const miniCheck = newVisited & minimumPaths[i];
    //        if(@popCount(u32, miniCheck) < newMaxVisit)
    //        {
    //            canExit = true;
    //            break;
    //        }
    //    }
    //    if(!canExit)
    //    {
    //        return 0;
    //    }
    //}

    var i:u8 = connStartIndex[index & 31];
    const connCount = i + connCounts[index & 31];
    var paths: u32 = 0;
    while(i < connCount) : (i += 1)
    {
        paths += checkConnection(newVisited, allConns[i], newMaxVisit);
    }

    const multiplier = ((index >> 5) & 7) + 1;
    return paths * multiplier;
}

