const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;



pub fn day21(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;

    var p1StartPos: u32 = 0;
    var p2StartPos: u32 = 0;
    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        const line1 = lines.next().?;
        const line2 = lines.next().?;

        p1StartPos = parseStart(line1);
        p2StartPos = parseStart(line2);
    }
    // Part A
    {
        var rollsTotal: u32 = 0;
        var turn: u32 = 0;
        var p1Score: u32 = 0;
        var p2Score: u32 = 0;
        var p1Pos = p1StartPos;
        var p2Pos = p2StartPos;
        while(p1Score < 1000 and p2Score < 1000)
        {
            const addedPos = 3 * ((rollsTotal + 1) % 100 + 1);

            p1Pos = (p1Pos - 1 + addedPos * (1 - turn)) % 10 + 1;
            p1Score += p1Pos * (1 - turn);

            p2Pos = (p2Pos - 1 + addedPos * turn) % 10 + 1;
            p2Score += p2Pos * turn;

            rollsTotal += 3;
            turn = 1 - turn;
        }

        if(p1Score >= 1000)
        {
            resultA = p2Score * rollsTotal;
        }
        else
        {
            resultA = p1Score * rollsTotal;
        }
    }

    // Part B
    {
        // Possible outcomes 1+1+1, 1+1+2, 1+1+3, 1+2+1...3+3+3
        var outcomes = std.mem.zeroes([10]u32);

        var x: u32 = 1;
        while(x <= 3) : (x += 1)
        {
            var y: u32 = 1;
            while(y <= 3) : (y += 1)
            {
                var z: u32 = 1;
                while(z <= 3) : (z += 1)
                {
                    outcomes[x + y + z] += 1;
                }
            }
        }

        // just set the p1wins and p2wins to invalid values
        {
            var i: usize = 0;
            while(i < games.len) : (i += 1)
            {
                var j: usize = 0;
                while(j < games[i].len) : (j += 1)
                {
                    var k: usize = 0;
                    while(k < games[i][j].len) : (k += 1)
                    {
                        var l: usize = 0;
                        while(l < games[i][j][k].len) : (l += 1)
                        {
                            games[i][j][k][l].p1Wins = 0xffff_ffff_ffff_ffff;
                            games[i][j][k][l].p2Wins = 0xffff_ffff_ffff_ffff;
                        }
                    }
                }
            }
        }
        // keep position 0..9
        const game = playRound(&outcomes, p1StartPos - 1, p2StartPos - 1, 0, 0);
        resultB = @maximum(game.p1Wins, game.p2Wins);
    }

    const res = try std.fmt.bufPrint(printBuffer, "Day 21-1: Rolls and multiply: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 21-2: Most wins: {}\n", .{resultB});
    return res.len + res2.len;
}

const Game = struct
{
    p1Wins: u64,
    p2Wins: u64,
};
// score p1,p2, pos p1,p2
var games: [21][21][10][10]Game = undefined;

fn playRound(outcomes: []const u32, p1Pos: u32, p2Pos: u32, p1Score: u32, p2Score: u32) Game
{
    var game = Game {.p1Wins = 0, .p2Wins = 0 };

    var rolls: u32 = 3;
    // only values 3..9 are ok.
    while(rolls <= 9) : (rolls += 1)
    {
        // pos 0..9 gives score 1..10
        var p1P = (p1Pos + rolls) % 10;
        var p1S = p1Score + p1P + 1;

        if(p1S >= 21)
        {
            game.p1Wins += outcomes[rolls];
            continue;
        }

        // swapping order
        if(games[p2Score][p1S][p2Pos][p1P].p1Wins == 0xffff_ffff_ffff_ffff)
            games[p2Score][p1S][p2Pos][p1P] = playRound(outcomes, p2Pos, p1P, p2Score, p1S);

        // swap order
        const subGame = games[p2Score][p1S][p2Pos][p1P];
        game.p1Wins += subGame.p2Wins * outcomes[rolls];
        game.p2Wins += subGame.p1Wins * outcomes[rolls];
    }
    return game;
}


// in case start is multiple digits
fn parseStart(line: []const u8) u32
{
    var i: usize = line.len - 1;
    while(i > 0 and std.ascii.isDigit(line[i])) : (i -= 1) {}

    var num: u32 = 0;
    i += 1;
    while(i < line.len and std.ascii.isDigit(line[i])) : (i += 1)
        num = num * 10 + line[i] - '0';
    return num;
}