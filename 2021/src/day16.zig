const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const bytesMax: u32 = 1000;

pub fn day16(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    var bytes = std.mem.zeroes([bytesMax]u8);
    var byteCount: usize = 0;
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");
        while (lines.next()) |line|
        {
            var i: usize = 0;
            byteCount = line.len / 2;
            while(i < line.len) : (i += 2)
            {
                const byte = readByte(line[i..i+2]);
                bytes[i / 2] = byte;
            }
        }
    }
    var dataIter = DataIter{.bytes = bytes[0..byteCount], .bitIndex = 0};
    const packet = readPacket(&dataIter);

    var resultA: u64 = packet.sumOfVers;
    var resultB: u64 = packet.literalValue;

    const res =try std.fmt.bufPrint(printBuffer, "Day 16-1: Sum of versions: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 16-2: Literal value: {}\n", .{resultB});
    return res.len + res2.len;
}

const PacketData = struct
{
    sumOfVers: u64,
    literalValue: u64
};

const DataIter = struct
{
    bytes: []const u8,
    bitIndex: u32,

    pub fn readBits(self: *DataIter, amount: u32) u64
    {
        var result: u64 = 0;
        var i: u32 = 0;
        while(i < amount) : (i += 1)
            result = (result << 1) + self.getNextBit();

        return result;
    }
    pub fn getNextBit(self: *DataIter) u1
    {
        const byte = self.bytes[self.bitIndex >> @as(u5, 3)];
        const res: u1 = @intCast(u1, (byte >> @intCast(u3, 7 - (self.bitIndex & 7))) & 1);
        self.bitIndex += 1;
        return res;
    }
};


fn oper(liters: []u64, typeId: u64) u64
{
    if(liters.len == 0)
        return 0;
    var res: u64 = liters[0];
    if(typeId < 4)
    {
        for(liters) |value, i|
        {
            if(i == 0) continue;
            switch(typeId)
            {
                0 => res += value,
                1 => res *= value,
                2 => res = @minimum(res, value),
                3 => res = @maximum(res, value),
                else => {}
            }
        }
    }
    else
    {
        const left = liters[0];
        const right = liters[1];
        switch(typeId)
        {
            5 => res = if(left  > right) 1 else 0,
            6 => res = if(left  < right) 1 else 0,
            7 => res = if(left == right) 1 else 0,
            else => {}
        }
    }
    return res;
}

fn readPacket(dataIter: *DataIter) PacketData
{
    const ver = dataIter.readBits(3);
    const typeId = dataIter.readBits(3);

    var sumOfVers: u64 = ver;

    var literalValue: u64 = 0;
    if(typeId == 4)
    {
        while(true)
        {
            const bit = dataIter.getNextBit();
            literalValue = (literalValue << @as(u6, 4));
            literalValue += dataIter.readBits(4);
            if(bit == 0)
            {
                break;
            }
        }
    }
    else
    {
        var liters = std.mem.zeroes([1024]u64);
        var literCount: usize = 0;

        const bit = dataIter.getNextBit();
        if(bit == 0)
        {
            const totalLen = @intCast(u32, dataIter.readBits(15));

            const bitStart = dataIter.bitIndex;
            while(dataIter.bitIndex - bitStart < totalLen)
            {
                const packet = readPacket(dataIter);
                sumOfVers += packet.sumOfVers;

                liters[literCount] = packet.literalValue;
                literCount += 1;
            }
        }
        else
        {
            const totalPackets = dataIter.readBits(11);

            var i: u64 = 0;
            while(i < totalPackets) : (i += 1)
            {
                const packet = readPacket(dataIter);
                sumOfVers += packet.sumOfVers;

                liters[literCount] = packet.literalValue;
                literCount += 1;
            }
        }

        literalValue = oper(liters[0..literCount], typeId);
    }
    return PacketData{.sumOfVers = sumOfVers, .literalValue = literalValue};
}


fn readByte(data: []const u8) u8
{
    var res: u8 = 0;
    var i: u3 = 0;
    while(i < 2) : (i += 1)
    {
        var t = data[i];
        if(t >= '0' and t <= '9') t = t - '0';
        if(t >= 'a' and t <= 'f') t = t - 'a' + 10;
        if(t >= 'A' and t <= 'F') t = t - 'A' + 10;
        res = (res << @as(u3, 4)) + t;
    }
    return res;
}

