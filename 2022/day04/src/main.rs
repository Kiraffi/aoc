const RUN_AMOUNT:u32 = 1;

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day04.txt");

    let now = std::time::Instant::now();
    /*
    for _ in 0..RUN_AMOUNT - 1
    {
        day04_1(false, &_data);
        day04_2(false, &_data);
    }
    */
    day04_1(true, &_data);
    day04_2(true, &_data);
    println!("Day04 duration: {}us", now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}
fn get_rooms(line: &str) -> u128
{
    let values: Vec<i32> = line
        .split("-")
        .map(|x| x.parse::<i32>().unwrap())
        .collect();

    // Set bits up to second value.
    let result = (1u128 << values[1]) - 1;
    // Get first value bits and invert the bits. The first value needs to be reduced by 1,
    // so for example 1 becomes 0th bit, then -1 will make it 0. Inverting 0 is ~0, all bits set.
    let first_invert_bits = !((1u128 << (values[0] - 1)) - 1);
    // And them together to get the range from first to second bits
    let result = result & first_invert_bits;
    return result;
}

fn get_fully_overlapping(vals: Vec<u128>) -> u32
{
    let combined = vals[0] & vals[1];
    if (vals[0] & combined) == vals[0]
    {
        return 1u32;
    }
    if (vals[1] & combined) == vals[1]
    {
        return 1u32;
    }
    return 0u32;
}

fn get_partial_overlapping(vals: Vec<u128>) -> u32
{
    if (vals[0] & vals[1]) != 0
    {
        return 1u32
    }
    return 0u32;
}

fn get_result<F>(content: &str, func: F) -> u32
    where F: Fn(Vec<u128>) -> u32
{
    content.lines()
        .fold(0u32, |prev, line|
        {
            func(line
                .split(",")
                .map(|str_pair| { get_rooms(str_pair) })
                .collect()
            ) + prev
        })
}

fn day04_1(print_outcome: bool, content: &str)
{
    let total_score: u32 = get_result(content, get_fully_overlapping);
    if print_outcome
    {
        println!("Day 04-1: Fully overlapping room assignments: {}", total_score);
    }
}

fn day04_2(print_outcome: bool, content: &str)
{
    let total_score: u32 = get_result(content, get_partial_overlapping);
    if print_outcome
    {
        println!("Day 04-2: Any overlapping room assignments: {}", total_score);
    }
}
