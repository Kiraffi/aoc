
const RUN_AMOUNT:u32 = 1;

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day03.txt");

    let now = std::time::Instant::now();
    for _ in 0..RUN_AMOUNT - 1
    {
        day03_1(false, &_data);
        day03_2(false, &_data);
    }
    day03_1(true, &_data);
    day03_2(true, &_data);
    println!("Day03 duration: {}us", now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn evaluate_str_value_to_priority(value: u64) -> u32
{
    // mask lower 32 bits and shift 27 bits up.
    let upper_case = (value & ((1u64 << 32u64) - 1u64)) << 27;
    // take higher 32 bits and shift down by 31
    let lower_case = value >> 31;

    return (upper_case | lower_case).trailing_zeros();
}

fn calculate_str_value(line: &[u8]) -> u64
{
    let mut value: u64 = 0;
    for n in 0..line.len()
    {
        // Bitwise OR for the letter value
        // 'A' bit 0, and 'a' bit 32
        value |= 1u64 << (line[n] - 'A' as u8);
    }
    return value;
}

fn day03_1(print_outcome: bool, content: &str)
{
    let mut total_score: u32 = 0;
    for line in content.lines()
    {
        let half = line.len() / 2;
        let line_bytes = line.as_bytes();
        let left_chars = &line_bytes[0..half];
        let right_chars = &line_bytes[half..];

        let value: u64 = calculate_str_value(left_chars) & calculate_str_value(right_chars);
        total_score += evaluate_str_value_to_priority(value);
    }
    if print_outcome
    {
        println!("Day 03-1: Sum of priorities: {}", total_score);
    }
}

fn get_str(option: Option<&str>) -> &str
{
    return match option
    {
        Some(value) => value,
        None => ""
    }
}

fn day03_2(print_outcome: bool, content: &str)
{
    let mut total_score: u32 = 0;
    let mut lines = content.lines(); //.peekable();
    // while lines.peek() != None
    while let Some(l0) = lines.next()
    {
        let value: u64 = calculate_str_value(l0.as_bytes());

        let l1 = get_str(lines.next());
        let value = value & calculate_str_value(l1.as_bytes());

        let l2 = get_str(lines.next());
        let value = value & calculate_str_value(l2.as_bytes());

        total_score += evaluate_str_value_to_priority(value);
    }
    if print_outcome
    {
        println!("Day 03-2: Sum of priorities: {}", total_score);
    }
}
