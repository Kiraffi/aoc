
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "20";
const DATA: &'static str = include_str!("../../data/day20.txt");
const _TEST_DATA: &'static str = include_str!("../test_data.txt");

fn main()
{
    let now = std::time::Instant::now();
    /*
    for _ in 0..RUN_AMOUNT - 1
    {
        _ = part_a(&_data);
        _ = part_b(&_data);
    }
    */
    println!("Day {}-1: 1000ths numbers sum after 0: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: After 10 mixes sum is: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

struct Link
{
    value: i64,
    prev: i64,
    next: i64
}

fn shuffle(numbers: &mut Vec<Link>, mut index: i64)
{
    index %= numbers.len() as i64;
    if numbers[index as usize].value == 0
    {
        return;
    }
    let prev = numbers[index as usize].prev;
    let next = numbers[index as usize].next;
    numbers[prev as usize].next = next;
    numbers[next as usize].prev = prev;

    // Make number positive
    let mut amount = numbers[index as usize].value;
    let mut new_index = index;
    if amount < 0
    {
        amount = -amount + 1;
        amount %= numbers.len() as i64 - 1;
        for _ in 0..amount
        {
            new_index = numbers[new_index as usize].prev;
        }
    }
    else
    {
        amount %= numbers.len() as i64 - 1;
        for _ in 0..amount
        {
            new_index = numbers[new_index as usize].next;
        }
    }
    let new_next = numbers[new_index as usize].next;
    numbers[index as usize].next = new_next;
    numbers[index as usize].prev = new_index;
    numbers[new_index as usize].next = index;
    numbers[new_next as usize].prev = index;
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 3);
}

fn part_a(content: &'static str) -> i64
{
    let mut numbers: Vec<Link> = Vec::new();
    let mut zero = 0;
    for line in content.lines()
    {
        numbers.push(Link {
            value: line.parse::<i64>().unwrap(),
            prev: numbers.len() as i64 - 1,
            next: numbers.len() as i64 + 1 });
        if numbers.last().unwrap().value == 0
        {
            zero = numbers.len() - 1;
        }
    }
    numbers[0].prev = numbers.len() as i64 - 1;
    for i in 0..numbers.len()
    {
        shuffle(&mut numbers, i as i64);
    }
    let mut index = 0;

    index = zero;
    let mut sum = 0;
    for _ in 0..3
    {
        for _ in 0..1000
        {
            index = numbers[index].next as usize;
        }
        sum += numbers[index].value;
    }

    return sum;
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 1623178306);
}

fn part_b(content: &'static str) -> i64
{
    let mut numbers: Vec<Link> = Vec::new();
    let mut zero = 0;
    for line in content.lines()
    {
        numbers.push(Link {
            value: line.parse::<i64>().unwrap() * 811589153,
            prev: numbers.len() as i64 - 1,
            next: numbers.len() as i64 + 1 });
        if numbers.last().unwrap().value == 0
        {
            zero = numbers.len() - 1;
        }
    }
    numbers[0].prev = numbers.len() as i64 - 1;

    for _ in 0..10
    {
        for i in 0..numbers.len()
        {
            shuffle(&mut numbers, i as i64);
        }
    }

    let mut index = zero;
    let mut sum = 0;
    for _ in 0..3
    {
        for _ in 0..1000
        {
            index = numbers[index].next as usize;
        }
        sum += numbers[index].value;
    }

    return sum;
}
