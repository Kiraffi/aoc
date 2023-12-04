const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "06";

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day06.txt");

    let now = std::time::Instant::now();
    /*
    for _ in 0..RUN_AMOUNT - 1
    {
        part_a(false, &_data);
        part_b(false, &_data);
    }
    */
    part_a(true, &_data);
    part_b(true, &_data);
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn find_amount_unique(line: &str, amount: usize) -> usize
{
    let bytes = line.as_bytes();
    // Works only up to amount of 15, 4 bits per char, 32 chars memory.
    let mut char_bits = 0u128;
    for n in 0..bytes.len()
    {

        let new_value = bytes[n] - 'a' as u8;
        // Reserve 4 bits per char
        char_bits += 1u128 << (new_value * 4);
        if n >= amount
        {
            let old_value = bytes[n - amount] - 'a' as u8;
            char_bits -= 1u128 << (old_value * 4);
        }
        // Combine 2 upper bits and 2 lower bits in other words
        // For example
        //   0011 0101 0000 0110 0110 0100
        // | 0000 1101 0100 0001 1001 1010
        // = 0011 1101 0100 0011 1111 1110
        let mut char_unique_bits = char_bits | (char_bits >> 2);

        // Then move the result one more bit to right and or together,
        // this means that if any of the 4 bits is 1, the lowest bit will be 1
        //   0011 1101 0100 0011 1111 1110
        // | 0001 1110 1010 0001 1111 1111
        // = 0011 1111 1110 0011 1111 1111
        char_unique_bits |= char_unique_bits >> 1;

        // Finally mask every fourth bit, which means value 0x1
        //   0011 1111 1110 0011 1111 1111
        // & 0001 0001 0001 0001 0001 0001
        // = 0001 0001 0000 0001 0001 0001
        char_unique_bits &= 0x1111_1111_1111_1111_1111_1111_1111_1111u128;

        // And count set bits. Easy way
        //let count_bits = char_unique_bits.count_ones() as usize;

        // Do some bit operations to count the unique bits.
        // First move upper 64 bits and sum with lower 64 bits and take only 64 bits.
        let mut count_bits = (char_unique_bits + (char_unique_bits >> 64)) as u64;

        // Then from there sum every 4 bits to next 4 bits
        count_bits += count_bits >> 4;

        // Mask to take lower 4 from every 8 bits, since we have limit of 4 bits per char
        count_bits &= 0x0f0f_0f0f_0f0f_0f0fu64;

        // Then from here on just do halve the summed bits
        count_bits += count_bits >> 32;
        count_bits += count_bits >> 16;
        count_bits += count_bits >> 8;

        // Finally mask the lowest 8 bits
        count_bits &= 0xff;

        if count_bits as usize >= amount
        {
            return n + 1;
        }
    }
    return 0usize;
}

fn part_a(print_outcome: bool, content: &str)
{
    let chars = find_amount_unique(content, 4);
    if print_outcome
    {
        println!("Day {}-1: First occuring 4 diff chars: {}", DAY_STR, chars);
    }
}


fn part_b(print_outcome: bool, content: &str)
{
    let chars = find_amount_unique(content, 14);
    if print_outcome
    {
        println!("Day {}-2: First occuring 14 diff chars: {}", DAY_STR, chars);
    }
}
