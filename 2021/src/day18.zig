const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const bytesMax: u32 = 192;
const bytesLink: u32 = 128;


fn isNumber(c: u8) bool
{
    return c < Comma;
}

const Comma: u8 = 64;
const RightBracket: u8 = 96;
const LeftBracket: u8 = 128;


fn printList(newList: *List) void
{
    var iter2 = newList.getIter();
    while(iter2.isValid())
    {
        const c = @intCast(u8, iter2.getValue());
        printChar(c);
        _ = iter2.getNext();
    }
    print("\n", .{});
}

pub fn day18(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var str: [bytesMax]u8 = undefined; //std.mem.zeroes([bytesMax]u8);

    var parsedLines: [100][64]u8 = undefined;
    var parsedLineLens: [100]usize = undefined;
    var lineCount: usize = 0;

    var strLen: usize = 0;
    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        while(lines.next()) |line|
        {
            var tmpStr: []u8 = &parsedLines[lineCount];
            var tmpStrLen: usize = 0;
            var i: usize = 0;
            while(i < line.len) : (i += 1)
            {
                const c = line[i];
                switch(c)
                {
                    '[' => tmpStr[tmpStrLen] = LeftBracket,
                    ']' => tmpStr[tmpStrLen] = RightBracket,
                    ',' => tmpStr[tmpStrLen] = Comma,
                    '0'...'9' => tmpStr[tmpStrLen] = c - '0',
                    else => {}
                }
                tmpStrLen += 1;
            }
            parsedLineLens[lineCount] = tmpStrLen;
            lineCount += 1;
        }
    }

    var newList: List = undefined;
    newList.reset();

    var resultA: u64 = 0;
    // Part A this a bit seems slower for part A
    if(false)
    {
        // add '[' in front of the string
        {
            var i: usize = 0;
            while(i < lineCount - 1) : (i += 1)
            {
                str[i] = LeftBracket;
            }
            strLen = 0;
        }
        var i: usize = 0;

        while(i < lineCount) : (i += 1)
        {
            const start = lineCount - i - 1;
            const tmpStr: []u8 = &parsedLines[i];
            const tmpStrLen = parsedLineLens[i];

            if(i > 0)
            {
                // adding because of first bracket.
                strLen += 1;
                str[start + strLen] = Comma;
                strLen += 1;
            }
            std.mem.copy(u8, str[start + strLen..], tmpStr[0..tmpStrLen]);
            strLen += tmpStrLen;
            if(i > 0)
            {
                str[start + strLen] = RightBracket;
                strLen += 1;
            }
            if(i > 0)
                strLen = parseLine(str[lineCount - i - 1..], strLen);

        }

        resultA = evaluateString(&str, strLen);
    }

    // Part B, looks like this is a bit faster way.
    var resultB: u64 = 0;
    //var loop: u32 = 0;
    //while(loop < 10) : (loop += 1)
    if(false)
    {

        var maxNumber: u64 = 0;
        var i: usize = 0;
        while(i < lineCount) : (i += 1)
        {
            const tmpStr: []u8 = &parsedLines[i];
            const tmpStrLen = parsedLineLens[i];
            var j: usize = i + 1;
            while(j < lineCount) : (j += 1)
            {
                const tmpStr2: []u8 = &parsedLines[j];
                const tmpStrLen2 = parsedLineLens[j];

                {
                    str[0] = LeftBracket;
                    strLen = 1;
                    std.mem.copy(u8, str[strLen..], tmpStr[0..tmpStrLen]);
                    strLen += tmpStrLen;
                    str[strLen] = Comma;
                    strLen += 1;

                    std.mem.copy(u8, str[strLen..], tmpStr2[0..tmpStrLen2]);
                    strLen += tmpStrLen2;
                    str[strLen] = RightBracket;
                    strLen += 1;
                    strLen = parseLine(&str, strLen);
                    maxNumber = @maximum(maxNumber, evaluateString(&str, strLen));
                }

                {
                    str[0] = LeftBracket;
                    strLen = 1;
                    std.mem.copy(u8, str[strLen..], tmpStr2[0..tmpStrLen2]);
                    strLen += tmpStrLen2;
                    str[strLen] = Comma;
                    strLen += 1;

                    std.mem.copy(u8, str[strLen..], tmpStr[0..tmpStrLen]);
                    strLen += tmpStrLen;
                    str[strLen] = RightBracket;
                    strLen += 1;
                    strLen = parseLine(&str, strLen);
                    maxNumber = @maximum(maxNumber, evaluateString(&str, strLen));
                }
            }
        }
        resultB = maxNumber;
    }

    // This seems a bit faster for part A
    if(true)
    {
        var i: usize = 0;
        while(i < lineCount) : (i += 1)
        {
            const tmpStr: []u8 = &parsedLines[i];
            const tmpStrLen = parsedLineLens[i];

            if(i > 0)
            {
                newList.getIter().push(LeftBracket, true);
            }

            var iter = newList.getIterLast();
            if(i > 0)
            {
                iter.push(Comma, false);
            }

            iter.pushArray(u8, tmpStr[0..tmpStrLen], false);

            if(i > 0)
            {
                newList.getIterLast().push(RightBracket, false);
            }
            parseLine2(&newList);
        }
        resultA = evaluateString2(&newList);
    }

    // this seems roughly same for part B, slower.
    //var loop: u32 = 0;
    //while(loop < 10) : (loop += 1)
    if(true)
    {
        var maxNumber: u64 = 0;
        var i: usize = 0;
        while(i < lineCount) : (i += 1)
        {
            const tmpStr: []u8 = &parsedLines[i];
            const tmpStrLen = parsedLineLens[i];
            var j: usize = i + 1;
            while(j < lineCount) : (j += 1)
            {
                const tmpStr2: []u8 = &parsedLines[j];
                const tmpStrLen2 = parsedLineLens[j];

                {
                    newList.reset();
                    var iter = newList.getIter();
                    iter.push(LeftBracket, false);
                    iter.pushArray(u8, tmpStr[0..tmpStrLen], false);
                    iter.push(Comma, false);
                    iter.pushArray(u8, tmpStr2[0..tmpStrLen2], false);
                    iter.push(RightBracket, false);
                    parseLine2(&newList);
                    maxNumber = @maximum(maxNumber, evaluateString2(&newList));
                }

                {
                    newList.reset();
                    var iter = newList.getIter();
                    iter.push(LeftBracket, false);
                    iter.pushArray(u8, tmpStr2[0..tmpStrLen2], false);
                    iter.push(Comma, false);
                    iter.pushArray(u8, tmpStr[0..tmpStrLen], false);
                    iter.push(RightBracket, false);
                    parseLine2(&newList);
                    maxNumber = @maximum(maxNumber, evaluateString2(&newList));
                }
            }
        }
        resultB = maxNumber;
    }

    const res =try std.fmt.bufPrint(printBuffer, "Day 18-1: Sum: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 18-2: Maximum value: {}\n", .{resultB});
    return res.len + res2.len;
}

const Stack = struct
{
    leftValue: u32,
    rightValue: u32,
};

const Node = struct
{
    const InvalidIndex: u8 = 0xff; //0xffff_ffff_ffff_ffff;

    prevIndex: u8,
    nextIndex: u8,
};

const List = struct
{
    nodes: [bytesLink]Node,
    data: [bytesLink]u8,

    head: u8,
    tail: u8,
    freeNodeHead: u8,
    size: u8,

    pub fn reset(self: *List) void
    {
        //var list = List{.head = Node.InvalidIndex, .tail = Node.InvalidIndex,
        //    .size = 0, .freeNodeHead = 0, .capasity = bytesMax, .nodes = undefined };

        self.head = Node.InvalidIndex;
        self.tail = Node.InvalidIndex;
        self.size = 0;
        self.freeNodeHead = 0;

        var i: u8 = 1;
        while(i < bytesLink - 1) : (i += 1)
        {
            self.nodes[i].nextIndex = i + 1; //@intCast(u8, (i + bytesMax + 1) % (bytesMax));
            self.nodes[i].prevIndex = i - 1; //@intCast(u8, (i + bytesMax - 1) % (bytesMax));
        }
        self.nodes[0].nextIndex = 1;
        self.nodes[0].prevIndex = bytesLink - 1;

        self.nodes[bytesLink - 1].nextIndex = 0;
        self.nodes[bytesLink - 1].prevIndex = bytesLink - 2;

        //return list;
    }

    pub fn getIter(self: *List) ListIter
    {
        return ListIter{.list = self, .index = self.head };
    }
    pub fn getIterLast(self: *List) ListIter
    {
        return ListIter{.list = self, .index = self.tail };
    }
};

const ListIter = struct
{
    list: *List,
    index: u8,

    pub fn pop(self: *ListIter, goToPrevious: bool) void
    {
        if(self.index == Node.InvalidIndex)
            return;
        //print("popping: {}\n", .{self.index});

        var currNode = &self.list.nodes[self.index];
        const nextIndex = currNode.nextIndex;
        const prevIndex = currNode.prevIndex;

        if(prevIndex != Node.InvalidIndex)
            self.list.nodes[prevIndex].nextIndex = nextIndex;
        if(nextIndex != Node.InvalidIndex)
            self.list.nodes[nextIndex].prevIndex = prevIndex;
        self.list.size -= 1;

        if(self.list.head == self.index)
            self.list.head = nextIndex;

        if(self.list.tail == self.index)
            self.list.tail = prevIndex;

        self.list.nodes[self.index].prevIndex = Node.InvalidIndex;
        self.list.nodes[self.index].nextIndex = self.list.freeNodeHead;

        self.list.freeNodeHead = self.index;
        if(goToPrevious)
        {
            self.index = prevIndex;
        }
        else
        {
            self.index = nextIndex;
        }
    }

    pub fn push(self: *ListIter, data: u8, before: bool) void
    {
        var newNodeIndex = self.list.freeNodeHead;
        // no room....
        if(newNodeIndex == Node.InvalidIndex)
        {
            //print("LIST IS FULL\n", .{});
            return;
        }

        self.list.size += 1;
        self.list.freeNodeHead = self.list.nodes[newNodeIndex].nextIndex;

        self.list.data[newNodeIndex] = data;
        self.list.nodes[newNodeIndex].prevIndex = Node.InvalidIndex;
        self.list.nodes[newNodeIndex].nextIndex = Node.InvalidIndex;


        if(self.index != Node.InvalidIndex)
        {
            if(before)
            {
                const prevIndex = self.list.nodes[self.index].prevIndex;

                self.list.nodes[self.index].prevIndex = newNodeIndex;
                self.list.nodes[newNodeIndex].nextIndex = self.index;
                self.list.nodes[newNodeIndex].prevIndex = prevIndex;
                if(prevIndex != Node.InvalidIndex)
                    self.list.nodes[prevIndex].nextIndex = newNodeIndex;
            }
            else
            {
                const nextIndex = self.list.nodes[self.index].nextIndex;

                self.list.nodes[self.index].nextIndex = newNodeIndex;
                self.list.nodes[newNodeIndex].nextIndex = nextIndex;
                self.list.nodes[newNodeIndex].prevIndex = self.index;
                if(nextIndex != Node.InvalidIndex)
                    self.list.nodes[nextIndex].prevIndex = newNodeIndex;
            }
        }
        else if(self.list.tail != Node.InvalidIndex)
        {
            self.list.nodes[self.list.tail].nextIndex = newNodeIndex;
            self.list.nodes[newNodeIndex].prevIndex = self.list.tail;
            self.list.tail = newNodeIndex;
        }


        if(before and self.list.head == self.index)
        {
            self.list.head = newNodeIndex;
        }
        else if(!before and self.list.tail == self.index)
        {
            self.list.tail = newNodeIndex;
        }

        if(self.list.tail == Node.InvalidIndex)
            self.list.tail = newNodeIndex;

        if(self.list.head == Node.InvalidIndex)
            self.list.head = newNodeIndex;

        self.index = newNodeIndex;
    }

    pub fn pushArray(self: *ListIter, comptime T: type, data: []T, before: bool) void
    {
        var newNodeIndex = self.list.freeNodeHead;
        // no room....
        if(newNodeIndex == Node.InvalidIndex)
        {
            //print("LIST IS FULL\n", .{});
            return;
        }
        if(data.len == 0 or bytesLink - self.list.size < data.len)
            return;

        self.list.data[newNodeIndex] = data[0];
        self.list.nodes[newNodeIndex].prevIndex = Node.InvalidIndex;

        const startIndex = newNodeIndex;
        var prevNodeIndex = newNodeIndex;
        self.list.size += 1;

        var i: usize = 1;
        while(i < data.len) : (i += 1)
        {
            self.list.size += 1;
            newNodeIndex = self.list.nodes[prevNodeIndex].nextIndex;

            self.list.data[newNodeIndex] = data[i];
            self.list.nodes[newNodeIndex].prevIndex = prevNodeIndex;
            prevNodeIndex = newNodeIndex;
        }

        self.list.freeNodeHead = self.list.nodes[newNodeIndex].nextIndex;
        self.list.nodes[newNodeIndex].nextIndex = Node.InvalidIndex;
        if(self.index != Node.InvalidIndex)
        {
            if(before)
            {
                const prevIndex = self.list.nodes[self.index].prevIndex;

                self.list.nodes[self.index].prevIndex = startIndex;
                self.list.nodes[newNodeIndex].nextIndex = self.index;
                self.list.nodes[startIndex].prevIndex = prevIndex;
                if(prevIndex != Node.InvalidIndex)
                    self.list.nodes[prevIndex].nextIndex = startIndex;
            }
            else
            {
                const nextIndex = self.list.nodes[self.index].nextIndex;

                self.list.nodes[self.index].nextIndex = startIndex;
                self.list.nodes[newNodeIndex].nextIndex = nextIndex;
                self.list.nodes[startIndex].prevIndex = self.index;
                if(nextIndex != Node.InvalidIndex)
                    self.list.nodes[nextIndex].prevIndex = newNodeIndex;
            }
        }
        else if(self.list.tail != Node.InvalidIndex)
        {
            self.list.nodes[self.list.tail].nextIndex = newNodeIndex;
            self.list.nodes[startIndex].prevIndex = self.list.tail;
            self.list.tail = newNodeIndex;
        }


        if(before and self.list.head == self.index)
        {
            self.list.head = startIndex;
        }
        else if(!before and self.list.tail == self.index)
        {
            self.list.tail = newNodeIndex;
        }

        if(self.list.tail == Node.InvalidIndex)
            self.list.tail = newNodeIndex;

        if(self.list.head == Node.InvalidIndex)
            self.list.head = startIndex;

        self.index = newNodeIndex;
    }


    pub fn isValid(self: *ListIter) bool
    {
        return self.index != Node.InvalidIndex;
    }

    pub fn getNext(self: *ListIter) u8
    {
        if(self.index == Node.InvalidIndex)
            return 0xff; //Node.InvalidIndex;

        self.index = self.list.nodes[self.index].nextIndex;
        if(self.index == Node.InvalidIndex)
            return 0xff; //Node.InvalidIndex;

        return self.list.data[self.index];
    }

    pub fn getPrev(self: *ListIter) u8
    {
        const index = self.index;
        if(index == Node.InvalidIndex)
            return 0xff; //Node.InvalidIndex;
        const prevIndex = self.list.nodes[self.index].prevIndex;
        self.index = prevIndex;
        if(self.index == Node.InvalidIndex)
            return 0xff; //Node.InvalidIndex;

        return self.list.data[self.index];
    }

    pub fn getValue(self: *ListIter) u8
    {
        if(self.index == Node.InvalidIndex)
            return 0xff; // Node.InvalidIndex;
        return self.list.data[self.index];
    }
    pub fn setValue(self: *ListIter, data: u8) void
    {
        if(self.index == Node.InvalidIndex)
            return;
        self.list.data[self.index] = data;
    }

};

fn evaluateString(str: []u8, strLen: usize) u64
{
    // stack
    var stack: [128]Stack = undefined;
    var stackPos: usize = 0;
    stack[0].leftValue = 0x8000_0000;
    stack[0].rightValue = 0x8000_0000;

    var i: usize = 0;
    while(i < strLen) : (i += 1)
    {
        const c = str[i];
        switch(c)
        {
            LeftBracket =>
            {
                stackPos += 1;
                stack[stackPos].leftValue = 0x8000_0000;
                stack[stackPos].rightValue = 0x8000_0000;
            },
            RightBracket =>
            {
                const lValue = stack[stackPos].leftValue;
                const rValue = stack[stackPos].rightValue;
                const value = lValue * 3 + rValue * 2;


                // reset...
                stack[stackPos].leftValue = 0x8000_0000;
                stack[stackPos].rightValue = 0x8000_0000;

                stackPos -= 1;

                const isLeft = (stack[stackPos].leftValue) >> @as(u5, 31);
                stack[stackPos].leftValue = lerp(stack[stackPos].leftValue, value, isLeft);
                stack[stackPos].rightValue = lerp(stack[stackPos].rightValue, value, 1 - isLeft);

            },
            Comma =>
            {
            },

            // is number
            else =>
            {
                const isLeft = (stack[stackPos].leftValue) >> @as(u5, 31);
                stack[stackPos].leftValue = lerp(stack[stackPos].leftValue, c, isLeft);
                stack[stackPos].rightValue = lerp(stack[stackPos].rightValue, c, 1 - isLeft);
            }
        }
    }
    return stack[0].leftValue;
}

fn lerp(left: u32, right: u32, lerpValue: u32) u32
{
    return left * (1 - lerpValue) + right * (lerpValue);
}



fn evaluateString2(list: *List) u64
{
    // stack
    var stack: [128]Stack = undefined;
    var stackPos: usize = 0;
    stack[0].leftValue = 0x8000_0000;
    stack[0].rightValue = 0x8000_0000;

    var iter = list.getIter();
    while(iter.isValid())
    {
        const c = iter.getValue();
        switch(c)
        {
            LeftBracket =>
            {
                stackPos += 1;
                stack[stackPos].leftValue = 0x8000_0000;
                stack[stackPos].rightValue = 0x8000_0000;
            },
            RightBracket =>
            {
                const lValue = stack[stackPos].leftValue;
                const rValue = stack[stackPos].rightValue;
                const value = lValue * 3 + rValue * 2;


                // reset...
                stack[stackPos].leftValue = 0x8000_0000;
                stack[stackPos].rightValue = 0x8000_0000;

                stackPos -= 1;

                const isLeft = (stack[stackPos].leftValue) >> @as(u5, 31);
                stack[stackPos].leftValue = lerp(stack[stackPos].leftValue, value, isLeft);
                stack[stackPos].rightValue = lerp(stack[stackPos].rightValue, value, 1 - isLeft);

            },
            Comma =>
            {
            },

            // is number
            else =>
            {
                const isLeft = (stack[stackPos].leftValue) >> @as(u5, 31);
                stack[stackPos].leftValue = lerp(stack[stackPos].leftValue, c, isLeft);
                stack[stackPos].rightValue = lerp(stack[stackPos].rightValue, c, 1 - isLeft);
            }
        }
        _ = iter.getNext();
    }
    return stack[0].leftValue;
}





// Using some possible indexing system would probably be faster like linked list.
fn parseLine2(list: *List) void
{
    var actionHappened = true;
    var values: [4]u8 = undefined;
    values[1] = Comma;
    values[3] = RightBracket;

    while(actionHappened)
    {
        actionHappened = false;

        var bracketCount: u32 = 0;
        //var rightMem: u64 = 0;

        var iter = list.getIter();
        while(iter.isValid())
        {
            const c = iter.getValue();
            bracketCount += (c >> @as(u3, 7));
            if(c == RightBracket)
            {
                if(bracketCount > 4)
                {
                    iter.pop(true);
                    var right = iter.getValue();
                    iter.pop(true);
                    iter.pop(true);
                    var left = iter.getValue();
                    iter.pop(true);
                    // add to left
                    {
                        var iter2 = iter;
                        while(iter2.isValid() and !isNumber(iter2.getValue()))
                        {
                            _ = iter2.getPrev();
                        }
                        if(iter2.isValid() and isNumber(iter2.getValue()))
                        {
                            const value = iter2.getValue() + left;
                            iter2.setValue(value);
                        }
                    }
                    // add to right
                    {
                        var iter2 = iter;
                        while(iter2.isValid() and !isNumber(iter2.getValue()))
                        {
                            _ = iter2.getNext();
                        }
                        if(iter2.isValid() and isNumber(iter2.getValue()))
                        {
                            iter2.setValue(iter2.getValue() + right);
                        }
                    }
                    iter.setValue(0);
                    //bracketCount -= 1;
                    //printList(list);
                    //actionHappened = true;
                    //break;
                }
                bracketCount -= 1;
            }
            _ = iter.getNext();
        }

        if(!actionHappened)
        {
            iter = list.getIter();
            while(iter.isValid())
            {
                const c = iter.getValue();
                if(isNumber(c))
                {
                    if(c > 9)
                    {
                        //printList(list);

                        const numL = c / 2;
                        const numR = c - numL;
                        values[0] = numL;
                        //values[1] = Comma;
                        values[2] = numR;
                        //values[3] = RightBracket;

                        iter.setValue(LeftBracket);
                        iter.pushArray(u8, &values, false);
                        //printList(list);
                        //print("\n",.{});
                        //iter.push(numL, false);
                        //iter.push(Comma, false);
                        //iter.push(numR, false);
                        //iter.push(RightBracket, false);
                        actionHappened = true;
                        break;
                    }
                }
                _ = iter.getNext();
            }
        }
    }

}






// Using some possible indexing system would probably be faster like linked list.
fn parseLine(str: []u8, strsLen: usize) usize
{
    var strLen = strsLen;
    var actionHappened = true;

    var strIndex: usize = 0;

    var tmpStr: [bytesMax]u8 = undefined;
    var tmpStrLen: usize = 0;

    while(actionHappened)
    {
        actionHappened = false;

        var fromStr = if(strIndex == 0) str else &tmpStr;
        var toStr = if(strIndex == 0) &tmpStr else str;

        var fromStrLen = if(strIndex == 0) &strLen else &tmpStrLen;
        var toStrLen = if(strIndex == 0) &tmpStrLen else &strLen;

        toStrLen.* = 0;
        var bracketCount: u32 = 0;

        var i: u32 = 0;
        var rightMem: u8 = 0;
        while(i < fromStrLen.*) : (i += 1)
        {
            const c = fromStr[i];
            toStr[toStrLen.*] = c;
            toStrLen.* += 1;

            if(c == LeftBracket)
            {
                bracketCount += 1;
            }
            else if(c == RightBracket)
            {
                if(bracketCount > 4)// and !actionHappened)
                {
                    actionHappened = true;

                    // cos we havent done any actions, its [numA, numB] always.
                    rightMem = fromStr[i - 1];
                    var left: u8 = fromStr[i - 3];
                    toStrLen.* -= 5;

                    var j = toStrLen.*;
                    while( !isNumber(toStr[j]) and j > 0) : (j -= 1) {}

                    if(isNumber(toStr[j]))
                    {
                        toStr[j] += left;
                    }

                    toStr[toStrLen.*] = 0;
                    toStrLen.* += 1;
                }
                bracketCount -= 1;
            }


            else if(isNumber(c))
            {
                var num = c;
                num += rightMem;
                rightMem = 0;
                toStr[toStrLen.* - 1] = num;
            }
        }
        var exploded = false;
        i = 0;
        fromStrLen.* = 0;
        while(i < toStrLen.*) : (i += 1)
        {
            const c = toStr[i];
            fromStr[fromStrLen.*] = c;
            fromStrLen.* += 1;
            if(isNumber(c))
            {
                var num = c;
                if(num > 9 and !exploded)
                {
                    // if level == 3... it will instantly collapse next round.
                    actionHappened = true;
                    const numL = num / 2;
                    const numR = num - numL;
                    fromStr[fromStrLen.* - 1] = LeftBracket;
                    fromStr[fromStrLen.* + 0] = numL;
                    fromStr[fromStrLen.* + 1] = Comma;
                    fromStr[fromStrLen.* + 2] = numR;
                    fromStr[fromStrLen.* + 3] = RightBracket;
                    fromStrLen.* += 4;
                    exploded = true;
                }
                else
                {
                    fromStr[fromStrLen.* - 1] = num;
                }
            }
        }


        //if(checkExplosion)
        //{
        //    checkExplosion = false;
        //}
        //else if(!actionHappened and !checkExplosion)
        //{
        //    checkExplosion = true;
        //}
        //strIndex = (strIndex + 1) % 2;
    }
    //if(strIndex % 2 == 1)
    //{
    //    std.mem.copy(u8, str, tmpStr[0..tmpStrLen]);
    //    strLen = tmpStrLen;
    //}
    return strLen;
}

fn printString(str: []u8) void
{
    var i: usize = 0;
    while(i < str.len) : (i += 1)
    {
        printChar(str[i]);
    }
    print("\n", .{});
}


fn printChar(c: u8) void
{
    switch(c)
    {
        LeftBracket =>  print("[", .{}),
        RightBracket =>  print("]", .{}),
        Comma => print(",", .{}),
        else =>  print("{}", .{c})
    }
}