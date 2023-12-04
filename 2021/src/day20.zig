const std = @import("std");

//const print = std.log.info;
const print = std.debug.print;

const ImageSize: u32 = 256;

const ImageOffset: u32 = 96;
pub fn day20(_: *std.mem.Allocator, inputFile: []const u8, printBuffer: []u8) anyerror! usize
{
    // seems faster to just use bytes instead of packing the bits into u64
    // actually pack 2 bits per index, since it checks window of 4x3 and returns 2 middle ones.
    var filter = std.mem.zeroes([512 * 2 * 2 * 2]u8);

    var resultA: u64 = 0;
    var resultB: u64 = 0;

    var image = std.mem.zeroes([ImageSize * ImageSize / 64]u64);
    var image2 = std.mem.zeroes([ImageSize * ImageSize / 64]u64);

    var rows: u32 = 0;
    var cols: u32 = 0;
    // Parse lines to strings....
    {
        var lines = std.mem.tokenize(u8, inputFile, "\r\n");

        // parsing the filter
        {
            var line = lines.next().?;
            var bit: u32 = 0;
            while(bit < line.len) : (bit += 1)
            {
                if(line[bit] == '#')
                {
                    filter[bit] = 1;
                }
                // otherwise '.' is zero
            }

            {
                const tmp = filter;
                //remap bits for filter, instead of 512, and 3 per row, use
                // 512 * 8 and 4 cols per row, but return 2 values. Possibly could do 3 but that
                // increases more complexity and size of the filter array, maybe faster?

                var i: u32 = 0;
                while(i < 512 * 8) : (i += 1)
                {
                    // 0b0111_0000_0000 >> 2 | 0b0111_0000 >> 1 | 0b0111 >> 0 and
                    // 0b1110_0000_0000 >> 3 | 0b1110_0000 >> 2 | 0b1110 >> 1 and
                    const firstBitSets: u32 = ((i & 0x700) >> 2) | ((i & 0x70) >> 1) | ((i & 0x7));
                    const secondBitSets: u32 = ((i & 0xe00) >> 3) | ((i & 0xe0) >> 2) | ((i & 0xe) >> 1);

                    filter[i] = (tmp[firstBitSets] << 0) | ( tmp[secondBitSets] << 1);
                }
            }
            // print filter
            //{
            //    bit = 0;
            //    while(bit < 512) : (bit += 1)
            //    {
            //        const c: u8 = if(sampleFilter(&filter, bit) == 1) '#' else '.';
            //        print("{c}", .{c});
            //    }
            //    print("\n", .{});
            //}

            // counting 3 << 0 + 3 << 3 + 3 << 6 +...+ 3 << 21 = 13176245766935394011
            //{
            //    var i: u6 = 0;
            //    var value: u64 = 0;
            //    while( i < 22) : (i += 1)
            //    {
            //        value += @as(u64, 3) << (i * 3);
            //    }
            //    print("value: {}\n", .{value});
            //}

        }

        {
            var row: u32 = ImageOffset;
            while(lines.next()) |line|
            {
                var col: u32 = ImageOffset;
                cols = @intCast(u32, line.len);
                var i: u32 = 0;
                while(i < line.len) : (i += 1)
                {
                    const bit: u64 = if(line[i] == '#') 1 else 0;
                    setSample(&image, col, row, bit);
                    col += 1;
                }
                row += 1;
                rows += 1;
            }

        }
        // printing
        {
            //printMap(&image, ImageOffset, ImageOffset, cols, rows);
            //print("\n", .{});
        }


        // check special case if going borders, like what is ... ... ... value, and ### ### ### value. like does it do some
        // interesting stuff, and then determine that all borders need to sample that
        var borderBit: u64 = filter[0] & 1;

        var loop: u32 = 0;
        while(loop < 50) : (loop += 1)
        {
            const sourceImage = if(loop % 2 == 0) &image else &image2;
            const destImage = if(loop % 2 == 0) &image2 else &image;

            const top: u32 = ImageOffset - loop - 1;
            const left: u32 = ImageOffset - loop - 1;
            const width: u32 = cols + (loop + 1) * 2;
            const height: u32 = rows + (loop + 1) * 2;

            // Handle borders separately, just fill the whole buffer with bit set or not
            {
                const value: u64 = borderBit * 0xffff_ffff_ffff_ffff;
                var index: u32 = 0;
                while(index < destImage.len) : (index += 1)
                {
                    destImage[index] = value;
                }
            }


            // Optimizing the thing by imagesize? Only check the middle
            // This means technically it can only grow one or max 2? pixels per modify

            // nastiness of having to read next bit... and previous from old 64bits
            var j = top;

            const heightAmount: u6 = 8;
            var rowValues: [heightAmount + 2]u64 = undefined;
            while(j < top + height) : (j += heightAmount)
            {
                var i = (left & ~(@as(u32, 63)));

                // Taking last bit from previous block.
                var filterIndex: u64 = 0;
                {
                    var bitRowIndex: u6 = 0;
                    while(bitRowIndex < heightAmount + 2) : (bitRowIndex += 1)
                    {
                        const value = sourceImage[((j - 1) * ImageSize + i)/ 64 - 1];
                        const value2 = (value) & 1;
                        const mergeValue = value2;
                        filterIndex |= mergeValue << ((heightAmount + 1 - bitRowIndex) * 4);
                    }
                }

                filterIndex = filterIndex << 1;
                {
                    var bitRowIndex: u6 = 0;
                    while(bitRowIndex < heightAmount + 2) : (bitRowIndex += 1)
                    {
                        rowValues[bitRowIndex] = sourceImage[((j - 1 + bitRowIndex) * ImageSize + i)/ 64];

                        // read only first bit...
                        const value1 = (rowValues[bitRowIndex]) >> 63;
                        const mergeValue = value1;
                        filterIndex |= mergeValue << ((heightAmount + 1 - bitRowIndex) * 4);
                    }
                }

                while(i < left + width) : (i += 64)
                {
                    //printMap(sourceImage, i - 1, j - 1, 3, 3);
                    var writeValue = std.mem.zeroes([16]u64);

                    var colIndex: u32 = 0;
                    // because of having to have preivous 8bytes value at same time, there is bad disconnect here.
                    while(colIndex < 62) : (colIndex += 2)
                    {
                        const bitColIndex = @intCast(u6, colIndex);
                        //printMap(sourceImage, i - 1 + bitColIndex, j - 1, 4, 3);

                        // keep bits keep 2 bits, and remove 3rd and 4th bit from every sequence
                        filterIndex &= @as(u64, 0x3333_3333_3333_3333);
                        // Then push the index by 2
                        filterIndex = filterIndex << 2;

                        var bitRowIndex: u6 = 0;
                        while(bitRowIndex < heightAmount + 2) : (bitRowIndex += 1)
                        {
                            const value1 = (rowValues[bitRowIndex] >> (62 - bitColIndex - 1) ) & 3;
                            filterIndex |= value1 << ((heightAmount + 1 - bitRowIndex) * 4);
                        }
                        bitRowIndex = 0;
                        while(bitRowIndex < heightAmount) : (bitRowIndex += 1)
                        {
                            // take 9 bits from the moving window.
                            const filterValue = sampleFilter(&filter, (filterIndex >> ((heightAmount - 1 - bitRowIndex) * 4)) & 4095);
                            writeValue[bitRowIndex] |= @intCast(u64, filterValue) << (62 - bitColIndex);
                        }
                    }

                    // last bit of next 64 bits needs the first bit read into filterindex.
                    filterIndex &= @as(u64, 0x3333_3333_3333_3333);
                    filterIndex = filterIndex << 2;

                    {
                        var bitRowIndex: u6 = 0;
                        while(bitRowIndex < heightAmount + 2) : (bitRowIndex += 1)
                        {
                            // read the last bit and push it....
                            const value1 = rowValues[bitRowIndex] & 1;

                            // Read the next block values into rowValues.
                            rowValues[bitRowIndex] = sourceImage[((j - 1 + bitRowIndex) * ImageSize + i)/ 64 + 1];

                            // read only first bit from the new line
                            const value2 = (rowValues[bitRowIndex]) >> 63;
                            // add the last bit from previous line and first from new one.
                            const mergeValue = (value1 << 1) | value2;
                            filterIndex |= mergeValue << ((heightAmount + 1 - bitRowIndex) * 4);

                        }
                    }

                    // write heightAmount lines of 64 bits.
                    {
                        var bitRowIndex: u6 = 0;
                        while(bitRowIndex < heightAmount) : (bitRowIndex += 1)
                        {
                            const filterValue = sampleFilter(&filter, (filterIndex >> ((heightAmount - 1 - bitRowIndex) * 4)) & 4095);
                            writeValue[bitRowIndex] |= @intCast(u64, filterValue);
                            destImage[((j + bitRowIndex) * ImageSize + i) / 64] = writeValue[bitRowIndex];
                        }
                    }
                }
            }


            const bit = borderBit * 4095;
            borderBit = sampleFilter(&filter, bit) & 1;

            if(loop == 1)
            {
                //printMap(destImage, top, left, width, height);
                resultA = countHashes(destImage, top, left, width, height);
            }
            if(loop == 49)
            {
                //printMap(destImage, top, left, width, height);
                resultB = countHashes(destImage, top, left, width, height);
            }
            //printMap(destImage, top - 5, left - 5, width + 10, height + 10);
            //print("\n", .{});
        }
    }


    const res =try std.fmt.bufPrint(printBuffer, "Day 20-1: Hashes: {}\n", .{resultA});
    const res2 = try std.fmt.bufPrint(printBuffer[res.len..], "Day 20-2: Hashes after 50 filters: {}\n", .{resultB});
    return res.len + res2.len;
}

fn getSample(image: []const u64, x: u32, y: u32) u32
{
    const index = x + y * ImageSize;
    return @intCast(u32, (image[index / 64] >> @intCast(u6, 63 - (index % 64))) & 1);
}
fn setSample(image: []u64, x: u32, y: u32, value: u64) void
{
    const index = x + y * ImageSize;
    const bit: u64 = @as(u64, 1) << @intCast(u6, 63 - (index % 64));
    image[index / 64] &= ~bit;
    image[index / 64] |= bit * value;
}

// should use countbits maybe
fn countHashes(image: []const u64, x: u32, y: u32, width: u32, height: u32) u32
{
    var result: u32 = 0;
    var j = x;
    while(j < y + height) : (j += 1)
    {
        var i = x;
        while(i < x + width) : (i += 1)
        {
            result += getSample(image, i, j);
        }
    }
    return result;
}

fn sampleFilter(filter: []const u8, bit: u64) u64
{
    return filter[bit];
}

fn printMap(image: []const u64, startX: u32, startY: u32, width: u32, height: u32) void
{
    var j: u32 = startY;
    while(j < startY + height) : (j += 1)
    {
        var i: u32 = startX;
        while(i < startX + width) : (i += 1)
        {
            const c = @intCast(u8, ((1 - getSample(image, i, j)) * ('.' - '#') + '#'));

            print("{c}", .{c});
        }
        print("\n", .{});
    }
}
