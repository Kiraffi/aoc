const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;


pub fn day4(alloc: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var lines = std.mem.tokenize(u8, inputFile, "\r\n");
    var bingoNumbers = std.ArrayList(u32).init(alloc);
    defer bingoNumbers.deinit();

    var numberIndices: [100]u32 = std.mem.zeroes([100]u32);
    // parse bingo pick numbers
    {
        var numberIndex: u32 = 0;
        const bingoNumberStr: []const u8 = lines.next().?;
        //print("str {s}", .{bingoNumberStr});

        var bingoNumberStrIter = std.mem.tokenize(u8, bingoNumberStr, ",");
        while (bingoNumberStrIter.next()) |numStr|
        {
            const num = try std.fmt.parseInt(u32, numStr, 10);
            numberIndices[num] = numberIndex;
            numberIndex += 1;
            try bingoNumbers.append(num);
        }
    }
    // apparently tokenize skips empty line
    //_ = lines.next();

    var lowestBoardWinningIndex:u32 = 255;
    var sumBoards:u32 = 0;

    var highestBoardWinningIndex:u32 = 0;
    var highestSumBoards:u32 = 0;

    {
        var board: [25]u32 = std.mem.zeroes([25]u32);
        var boardIndex: u32 = 0;
        var rowIndex: u32 = 0;
        while (lines.next()) |boardLineStr|
        {
            var boardNumberIter = std.mem.tokenize(u8, boardLineStr, " ");
            var colIndex: u32 = 0;
            while (boardNumberIter.next()) |boardNumber|
            {
                const num = try std.fmt.parseInt(u32, boardNumber, 10);
                board[colIndex + rowIndex * 5] = numberIndices[num];
                colIndex += 1;
            }

            rowIndex += 1;
            if(rowIndex >= 5)
            {

                var tmp: u32 = 0;
                var smallestColumnRow: u32 = 255;
                while(tmp < 5) : (tmp += 1)
                {

                    var minRow: u32 = board[tmp * 5];
                    var minCol: u32 = board[tmp];
                    var tmp2: u32 = 0;
                    while(tmp2 < 5) : (tmp2 += 1)
                    {
                        minRow = @maximum(minRow, board[tmp * 5 + tmp2]);
                        minCol = @maximum(minCol, board[tmp + tmp2 * 5]);
                    }

                    smallestColumnRow = @minimum(minCol, smallestColumnRow);
                    smallestColumnRow = @minimum(minRow, smallestColumnRow);
                }

                if(smallestColumnRow < lowestBoardWinningIndex)
                {
                    lowestBoardWinningIndex = smallestColumnRow;
                    sumBoards = calculateSum(&board, bingoNumbers.items, smallestColumnRow);
                }

                if(smallestColumnRow > highestBoardWinningIndex)
                {
                    highestBoardWinningIndex = smallestColumnRow;
                    highestSumBoards = calculateSum(&board, bingoNumbers.items, smallestColumnRow);
                }
                rowIndex = 0;
                boardIndex += 1;
            }

        }
    }

    const res = try std.fmt.bufPrint(printBuffer, "Day4-1: winning number: {d}, day 4-1 solution: {d}\n", .{ bingoNumbers.items[lowestBoardWinningIndex], sumBoards });
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day4-2: Last board winning number: {d}, day 4-2 solution: {d}\n", .{ bingoNumbers.items[highestBoardWinningIndex], highestSumBoards });
    return res.len + res2.len;
}

fn calculateSum(board: []u32, numbers: []u32, smallestIndex: u32) u32
{
    var sumBoard: u32 = 0;
    var i:u32 = 0;
    while(i < 25) : (i += 1)
    {
        if(board[i] > smallestIndex)
        {
            sumBoard += numbers[board[i]];
        }
    }
    sumBoard *= numbers[smallestIndex];
    return sumBoard;
}