const std = @import("std");
const print = std.debug.print;

pub fn main() anyerror!void
{
    //var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    //defer arena.deinit();
    //const allocator = &arena.allocator;

    const inputFile: []const u8 = @embedFile("../data.txt");

    //var bingoNumbers = std.ArrayList(u64).init(allocator);
    //defer bingoNumbers.deinit();


    //var lines = std.mem.tokenize(u8, inputFile, "\r\n");
    //while (lines.next()) |numStr|
    //{
    //    const num = try std.fmt.parseInt(u64, numStr, 10);
    //    try bingoNumbers.append(num);
    //}

    var cups: [9]u32 = undefined;

    {
        var i: u32 = 0;
        while(i < cups.len) : (i += 1)
        {
            // map values 1..9 to 0..8
            cups[i] = inputFile[i] - '1';
        }
    }
    if(true)
    {
        shuffleCups2(&cups, 100);
        var i:u32 = 0;
        var startIndex: u32 = 0;
        while(i < 9) : (i += 1)
        {
            if(cups[i] == 0)
            {
                startIndex = (i + 1) % 9;
                break;
            }
        }

        i = 0;
        while(i < 8) : (i += 1)
        {
            const index = (startIndex + i) % 9;
            print("{}", .{cups[index] + 1});
        }
        print("\n", .{});
    }
    if(true)
    {
        //var cups2: [1_000_000]u32 = undefined;
        var cups2: [1_000_000]u32 = undefined;
        const len = @intCast(u32, cups2.len);
        var i: u32 = 0;
        while(i < len) : (i += 1)
        {
            cups2[i] = i;
        }

        i = 0;
        while(i < cups.len) : (i += 1)
        {
            cups2[i] = cups[i];
        }


        //{
        //    var j: u32 = 0;
        //    while(j < len) : (j += 1)
        //    {
        //        if(findIndex(cups2[j + 1..], cups2[j]) != 0xffff_ffff)
        //        {
        //            print("matching found!\n", .{});
        //        }
        //    }
        //}

        //shuffleCups(&cups2, 10_000_000);
        shuffleCups2(&cups2, 10_000_000);

        i = 0;
        while(i < cups2.len) : (i += 1)
        {
            if(cups2[i] == 0)
            {
                const value1 = cups2[(i + 1) % len] + 1;
                const value2 = cups2[(i + 2) % len] + 1;
                print("1: {}, 2: {}\n", .{ value1, value2 });

                break;
            }
        }
    }
    //print("input file: {s}\n", .{inputFile});
    //print("bingo: {}\n", .{bingoNumbers.items[0]});

}

const Node = struct
{
    previous: u32,
    next: u32,
};

fn printCups(cups: []Node, amount: u32)void
{
    var i: u32 = 0;
    while(i < amount) : (i += 1)
    {
        print("index: {}, prev {}, next: {}\n", .{i, cups[i].previous, cups[i].next});
    }
}

fn shuffleCups2(cupsOrig: []u32, loops: u32) void
{
    const cupLen = @intCast(u32, cupsOrig.len);
    var cups: [1_000_000]Node = undefined;
    {
        var i: u32 = 0;
        while(i < cupLen) : (i += 1)
            cups[cupsOrig[i]] = Node{.previous = cupsOrig[(cupLen + i - 1) % cupLen], .next = cupsOrig[(i + 1) % cupLen]};
    }

    //532608147

    //508143267
    var currentCupValue: u32 = cupsOrig[0];
    var loop: u32 = 0;

    while(loop < loops) : (loop += 1)
    {
        //print("before: index: {}\n", .{currentCupValue});
        //printCups(&cups, 9);

        var previousValue = currentCupValue;

        var found = true;
        while(found)
        {
            found = false;

            previousValue = (cupLen + previousValue - 1) % cupLen;
            var j: u32 = 0;
            var node = cups[currentCupValue];
            while(j < 3 and !found) : (j += 1)
            {
                found = found or (node.next == previousValue);
                node = cups[node.next];
            }
        }

        {
            const tmpPrevNext = cups[previousValue].next;
            const moveIndexStart = cups[currentCupValue].next;
            const moveLast = cups[cups[moveIndexStart].next].next;
            const tmpAfterLast = cups[moveLast].next;

            // fix the start index of the moved.
            cups[previousValue].next = moveIndexStart;
            cups[moveIndexStart].previous = previousValue;

            // fix the end index of the moved.
            cups[moveLast].next = tmpPrevNext;
            cups[tmpPrevNext].previous = moveLast;

            // fix the original after move.
            cups[currentCupValue].next = tmpAfterLast;
            cups[tmpAfterLast].previous = currentCupValue;
        }
        currentCupValue = cups[currentCupValue].next;

        //print("after: index: {}\n", .{currentCupValue});
        //printCups(&cups, 9);

    }
    {
        var i: u32 = 1;
        var node: Node = cups[0];
        cupsOrig[0] = 0;
        while(i < cupsOrig.len) : (i += 1)
        {
            cupsOrig[i] = node.next;
            node = cups[node.next];
        }
    }
}



fn shuffleCups(cupsOrig: []u32, loops: u32) void
{
    var cups: [2][1_000_000]u32 = undefined;
    {
        var i: u32 = 0;
        while(i < cupsOrig.len) : (i += 1)
        {
            cups[0][i] = cupsOrig[i];
            cups[1][i] = cupsOrig[i];
        }

    }


    var currentCupIndex: u32 = 0;

    var cupsIndex: u32 = 0;
    //var newCupsIndex: u32 = 1;

    const cupLen = @intCast(u32, cupsOrig.len);

    //var highestWrites: u32 = 0;

    var loop: u32 = 0;

    while(loop < loops) : (loop += 1)
    {
        var oldCups = &cups[cupsIndex];
        //var newCups = &cups[newCupsIndex];

        var holdedValues: [3]u32 = undefined;

        const holdValue = oldCups[currentCupIndex];

        const lesser = getLesserNumber(oldCups, cupLen, currentCupIndex);

        {
            var j: u32 = 0;
            while(j < 3) : (j += 1)
            {
                const index = (currentCupIndex + 1 + j) % cupLen;
                holdedValues[j] = oldCups[index];
            }
        }

        //const previousIndexPtr = std.mem.indexOfScalar(u32, oldCups[0..cupLen], lesser);
        //const previousIndex = previousIndexPtr.?;
        const previousIndex = findIndex(oldCups[0..cupLen], lesser);


        const diff = (cupLen + previousIndex - currentCupIndex) % cupLen;
        //print("\nloop: {} = holdValue: {} holdvalueindex: {}, dest: {}, destIndex: {}, diff {}\n",
         //   .{loop,  holdValue + 1, currentCupIndex, lesser + 1, previousIndex, diff});

        var index: u32 = 0;
        var writeCount: u32 = 0;
        if(diff < cupLen / 2)
        {
            //print("Before 1: ", .{});
            //printNumbers(&cups[cupsIndex], cupLen);
            index = (currentCupIndex + 1) % cupLen;
            var source = (index + 3) % cupLen;
            while(oldCups[source] != lesser) : (index = (index + 1) % cupLen)
            {
                oldCups[index] = oldCups[source];
                source = (index + 4) % cupLen;
                writeCount += 1;
            }
            oldCups[index] = oldCups[source];
            index = (index + 1) % cupLen;
            //print("After 1:", .{});
            //printNumbers(&cups[cupsIndex], cupLen);
        }
        else
        {
            //print("Before 2: ", .{});
            //printNumbers(&cups[cupsIndex], cupLen);
            index = (currentCupIndex + 3) % cupLen;
            var source = currentCupIndex;
            //print("start index: {}, source; {}\n", .{index, source});
            while(oldCups[source] != lesser)
            {
                oldCups[index] = oldCups[source];
                index = (cupLen + index - 1) % cupLen;
                source = (cupLen + source - 1) % cupLen;
                writeCount += 1;
            }
            index = (source + 1) % cupLen;
            //print("After 2:", .{});
            //printNumbers(&cups[cupsIndex], cupLen);
        }
        {
            //print("writes: {}\n", .{writeCount});
            //print("index: {}\n", .{index});
            var i: u32 = 0;
            while(i < 3) : (i += 1)
            {
                oldCups[index] = holdedValues[i];
                index = (index + 1) % cupLen;
            }
        }
        //print("After writing from memory: ", .{});
        //printNumbers(&cups[cupsIndex], cupLen);

        //var i: u32 = 0;
        //var oldIndex: u32 = 0;
        //var writes: u32 = 0;
        //while(i < cupLen) : (i += 1)
        //{
        //    {
        //        var j: u32 = 0;
        //        while(j < 3) : (j += 1)
        //        {
        //            const cupIndex = (currentCupIndex + 1 + j) % cupLen;
        //            if(oldIndex == cupIndex)
        //            {
        //                oldIndex = (oldIndex + 1) % cupLen;
        //                writes += 1;
        //            }
        //        }
        //    }
//
        //    newCups[i] = oldCups[oldIndex];
        //    if(oldCups[oldIndex] == lesser)
        //    {
        //        if(loops > 100)
        //        {
        //            print("lesser loop: {}: {} \n", .{loop, i});
        //        }
        //        var j: u32 = 0;
        //        while(j < 3) : (j += 1)
        //        {
        //            i = (i + 1) % cupLen;
        //            const newIndex = (currentCupIndex + 1 + j) % cupLen;
        //            newCups[i] = oldCups[newIndex];
        //            writes += 1;
        //        }
        //    }
        //    highestWrites = @maximum(highestWrites, writes);
        //    oldIndex = (oldIndex + 1) % cupLen;
//
        //    if(writes == 6)
        //    {
        //        i = i + 1;
        //        break;
        //    }
        //
        //}
        //if(loops > 100)
        //{
        //    print("loop: {}: {} \n", .{loop, i});
        //}
        //if(i == oldIndex and i < cupLen / 2)// and i < cupLen / 2)
        //{
        //    cupsIndex = (cupsIndex + 1) % 2;
        //    newCupsIndex = (newCupsIndex + 1) % 2;
        //
        //    oldCups = &cups[cupsIndex];
        //    newCups = &cups[newCupsIndex];
//
        //    i = 0;
        //    while( i < oldIndex) : (i += 1)
        //    {
        //        newCups[i] = oldCups[i];
        //    }
        //
        //}
        //else
        //{
        //    while( i < cupLen) : (i += 1)
        //    {
        //        newCups[i] = oldCups[oldIndex];
        //        oldIndex = (oldIndex + 1) % cupLen;
        //    }
        //}

        {
            //const currentIndexPtr = std.mem.indexOfScalar(u32, oldCups[0..cupLen], holdValue);
            //var foundIndex = @intCast(u32, currentIndexPtr.?);
            //const previousIndex = previousIndexPtr.?;
            var foundIndex = findIndex(oldCups[0..cupLen], holdValue);
            currentCupIndex = (foundIndex + 1) % cupLen;
            //var i:u32 = 0;
            //while(i < cupLen) : (i += 1)
            //{
            //    if(oldCups[i] == holdValue)
            //    {
            //        currentCupIndex = (i + 1) % cupLen;
            //        break;
            //    }
            //}
        }
        //print("old currentcupiundex: {}\n", .{currentCupIndex});
        //if(currentCupIndex < previousIndex)
        //{
        //    currentCupIndex = (currentCupIndex + 1) % cupLen;
        //}
        //else
        //{
        //    currentCupIndex = (currentCupIndex + 4) % cupLen;
        //}
        //print("new currentcupiundex: {}\n", .{currentCupIndex});


        // cupsIndex = (cupsIndex + 1) % 2;
        // newCupsIndex = (newCupsIndex + 1) % 2;

        //print("after round {}: ", .{loop + 1});
        //printNumbers(&cups[cupsIndex], cupLen);
    }
    //print("highest writes ever: {}\n", .{highestWrites});
    {
        var i: u32 = 0;
        while(i < cupsOrig.len) : (i += 1)
        {
            cupsOrig[i] = cups[cupsIndex][i];
        }
    }
}

fn findIndex(cups: []u32, index: u32) u32
{
    var i:u32 = 0;
    const cupLen = @intCast(u32, cups.len);
    var found: u32 = 0;
    while(i < cupLen and found == 0) : (i += 8)
    {
        found |= if(cups[i + 0] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 1] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 2] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 3] == index) @as(u32, 1) else @as(u32, 0);

        found |= if(cups[i + 4] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 5] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 6] == index) @as(u32, 1) else @as(u32, 0);
        found |= if(cups[i + 7] == index) @as(u32, 1) else @as(u32, 0);
    }
    if(found != 0)
    {
        i -= 8;
        while(i < cupLen) : (i += 1)
        {
            if(cups[i] == index)
                return i;
        }
    }
    //print("not found?\n", .{});
    return 0xffff_ffff;
}

fn getLesserNumber(cups: []u32, cupLen: u32, currentCupIndex: u32) u32
{
    const holdValue = cups[currentCupIndex];

    // -1 with wrap around
    var lesser = (holdValue + cupLen - 1) % cupLen;
    {
        var exists = true;
        while(exists)
        {
            exists = false;
            var j: u32 = 0;
            while(j < 3) : (j += 1)
            {
                const checkIndex = (j + currentCupIndex + 1) % cupLen;
                if(lesser == cups[checkIndex])
                {
                    lesser = (lesser + cupLen - 1) % cupLen;
                    exists = true;
                }
            }
        }
    }
    return lesser;
}

fn printNumbers(numb: []const u32, len: usize) void
{
    var i: u32 = 0;
    while(i < len) : (i += 1)
    {
        print("{}", .{numb[i] + 1});
    }
    print("\n", .{});
}

test "basic test"
{
    try std.testing.expectEqual(10, 3 + 7);
}
