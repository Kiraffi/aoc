const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const Command = enum(u8) {
    inp,
    add,
    mul,
    div,
    mod,
    eql,
    invalidCmd
};

const Param = enum(u8) {
    x,
    y,
    z,
    w,
    value,
    invalidParam,
};

const Input = struct {
    cmd: Command,
    param1: Param,
    param2: Param,
    param2Value: i64,
};

var divs: [14]i64 = undefined;
var add1s: [14]i64 = undefined;
var add2s: [14]i64 = undefined;

var answer: i64 = 0;

fn parseRecursivelyFast(startZ: i64, depth: usize, dir: bool) i64
{
    var numb:  i64 = 9;
    var maxZ: i64 = 1;
    var tmpI: usize = depth;
    while(tmpI < 14) : (tmpI += 1)
    {
        maxZ *= divs[tmpI];
    }

    while(numb > 0) : (numb -= 1)
    {
        var z = startZ;
        const w = if(dir) 10 - numb else numb;
        var x:i64 = @rem(z, 26);
        z = @divTrunc(z, divs[depth]);
        x = if(x + add1s[depth] == w) 0 else 1;
        var y: i64 = 25 * x + 1;
        z *= y;
        y = (w + add2s[depth]) * x;
        z += y;
        if(depth < 13)
        {
            if(z > maxZ or z < 0)
                continue;
            const result = parseRecursivelyFast(z, depth + 1, dir);
            if(result >= 0)
            {
                var tmp: i64 = 13;
                var tmp2: i64 = w;
                while(tmp > depth) : (tmp -= 1)
                    tmp2 *= 10;
                return tmp2  + result;
            }
        }
        else
        {
            if(z == 0)
            {
                return if(dir) 10 - numb else numb;
            }
        }
    }
    return -1;
}

pub fn day24(alloc: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var resultA: u64 = 0;
    var resultB: u64 = 0;

    var commands = std.ArrayList(Input).init(alloc);
    defer commands.deinit();

    var commands2 = std.ArrayList(Input).init(alloc);
    defer commands2.deinit();

    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");

        while(lines.next()) |line|
        {
            var cmd = Input {.cmd = Command.invalidCmd, .param1 = Param.invalidParam,
                .param2 = Param.invalidParam, .param2Value = 0x7fff_ffff };
            var input = std.mem.tokenize(u8, line, " ");
            const cmdName = input.next().?;
            cmd.cmd = blk: {
                var command = Command.invalidCmd;
                // hmm needs word inline to work...
                inline for (@typeInfo(Command).Enum.fields) |field, i|
                {
                    if (std.mem.eql(u8, cmdName, field.name))
                    {
                        command = @intToEnum(Command, i);
                    }
                }
                break :blk command;
            };

            try parseParam(input.next().?, &cmd.param1, &cmd.param2Value);
            const param2 = input.next();
            if(param2 != null)
                try parseParam(param2.?, &cmd.param2, &cmd.param2Value);
            try commands.append(cmd);
        }
    }

    {
        // simplify
        var regs: [4]i64 = .{0, 0, 0, 0};
        for(commands.items) |cmd, i|
        {
            if((cmd.cmd == Command.div or cmd.cmd == Command.mul)
                and cmd.param2 == Param.value and cmd.param2Value == 1)
            {
                //print("{}: removing div/mul by 1 \n", .{i});
                continue;
            }
            _ = i;
            try commands2.append(cmd);
        }
        _ = regs;
    }


    {

        var i: usize = 0;
        while(i < 14) : (i += 1)
        {
            divs[i]  = @intCast(i64, commands.items[i * 18 + 4].param2Value);
            add1s[i] = @intCast(i64, commands.items[i * 18 + 5].param2Value);
            add2s[i] = @intCast(i64, commands.items[i * 18 + 15].param2Value);
        }


        //resultA = @intCast(u64, parseRecursively(&commands2, 0, 0, .{0, 0, 0, 0}, false));
        //resultB = @intCast(u64, parseRecursively(&commands2, 0, 0, .{0, 0, 0, 0}, true));
        resultA = @intCast(u64, parseRecursivelyFast(0, 0, false));
        resultB = @intCast(u64, parseRecursivelyFast(0, 0, true));

    }

    const res = try std.fmt.bufPrint(printBuffer, "Day 24-1: Highest valid value: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 24-2: Lowest value: {}\n", .{resultB});
    return res.len + res2.len;
}


fn parseRecursively(commands: *std.ArrayList(Input), startCommandIndex: usize,
    depth: usize, registerStart: [4]i64, dir: bool) i64
{
    var maxZ:i64 = 1;
    {
        var tmp: usize = if(depth > 0) depth - 1 else 0;
        while(tmp < 14) : (tmp += 1)
            maxZ *= divs[tmp];
    }
    var number: i64 = 9;
    while(number > 0) : (number -= 1)
    {
        var regs: [4]i64 = registerStart;

        var commandIndex = startCommandIndex;
        while(commandIndex < commands.items.len) : (commandIndex += 1)
        {
            const cmd = commands.items[commandIndex];
            const writeRegisterIndex = @enumToInt(cmd.param1);
            switch(cmd.cmd)
            {
                Command.inp => {
                    if(regs[2] < 0)
                    {
                        break;
                    }

                    var tmpNumber = if(dir) 10 - number else number;
                    if(commandIndex != startCommandIndex)
                    {
                        const result = parseRecursively(commands, commandIndex, depth + 1, regs, dir);
                        if(result >= 0)
                        {
                            var tmp: i64 = 13;
                            while(tmp > depth) : (tmp -= 1)
                                tmpNumber *= 10;
                            return tmpNumber  + result;

                        }
                        break;
                    }
                    if(regs[2] > maxZ * 1)
                        break;

                    regs[writeRegisterIndex] = tmpNumber;
                },
                Command.invalidCmd  => {
                    print("invalid command, unreachable\n", .{});
                    unreachable;
                },

                else =>
                {
                    const first = regs[writeRegisterIndex];
                    const second = if(cmd.param2 == Param.value) cmd.param2Value
                        else regs[@enumToInt(cmd.param2)];
                    switch(cmd.cmd)
                    {
                        Command.add => { regs[writeRegisterIndex] = first + second;  },
                        Command.mul => { regs[writeRegisterIndex] = first * second;  },
                        Command.div => {
                            if(second == 0)
                                break;
                            regs[writeRegisterIndex] = @divTrunc( first, second);
                        },
                        Command.mod => {
                            if(first < 0)
                                break;
                            regs[writeRegisterIndex] = @rem(first, second);
                        },
                        Command.eql => { regs[writeRegisterIndex] = if(first == second) 1 else 0; },

                        else => {}
                    }
                }
            }
        }
        if(regs[2] == 0 and depth == 13)
        {
            return if(dir) 10 - number else number;
        }

    }
    return -1;
}

fn parseParam(paramStr: []const u8, param: *Param, value: *i64) anyerror!void
{
    inline for (@typeInfo(Param).Enum.fields) |field, i|
    {
        if (std.mem.eql(u8, paramStr, field.name))
        {
            param.* = @intToEnum(Param, i);
            return;
        }
    }
    value.* = try std.fmt.parseInt(i64, paramStr, 10);
    param.* = Param.value;
}