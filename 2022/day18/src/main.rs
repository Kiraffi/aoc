use std::collections::VecDeque;


const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "18";
const DATA: &'static str = include_str!("../../data/day18.txt");
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
    println!("Day {}-1: Lava surface areas: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Lava exterior surface areas: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 64);
}

fn part_a(content: &'static str) -> i64
{
    let mut visible_sides = 0;
    let mut blocks: [[[u8; 22]; 22]; 22] = [[[0; 22]; 22]; 22];
    for line in content.lines()
    {
        let values = line.split(',').map(|s| { s.parse::<usize>().unwrap() }).collect::<Vec<usize>>();
        blocks[values[0] + 1][values[1] + 1][values[2] + 1] = 1;
        visible_sides += 6;
        if blocks[values[0] + 1][values[1] + 1][values[2] + 2] != 0 { visible_sides -= 2; }
        if blocks[values[0] + 1][values[1] + 2][values[2] + 1] != 0 { visible_sides -= 2; }
        if blocks[values[0] + 2][values[1] + 1][values[2] + 1] != 0 { visible_sides -= 2; }

        if blocks[values[0] + 1][values[1] + 1][values[2] + 0] != 0 { visible_sides -= 2; }
        if blocks[values[0] + 1][values[1] + 0][values[2] + 1] != 0 { visible_sides -= 2; }
        if blocks[values[0] + 0][values[1] + 1][values[2] + 1] != 0 { visible_sides -= 2; }

    }
    return visible_sides;
}

fn check_point(point: (i32, i32, i32), blocks: &[[[u8; 22]; 22]; 22]) -> bool
{
    if point.0 < 0 || point.0 >= 22
        || point.1 < 0 || point.1 >= 22
        || point.2 < 0 || point.2 >= 22
        {
            return false;
        }
    return blocks[point.0 as usize][point.1 as usize][point.2 as usize] != 0;
}

fn add(a: (i32, i32, i32), b: (i32, i32, i32)) -> (i32, i32, i32)
{
    return (a.0 + b.0, a.1 + b.1, a.2 + b.2);
}

fn calculate_exterior(blocks: &[[[u8; 22]; 22]; 22]) -> usize
{
    let mut visited: [[[u8; 22]; 22]; 22] = [[[0; 22]; 22]; 22];
    let mut area = 0;
    let mut deque: VecDeque<(i32, i32, i32)> = VecDeque::new();
    deque.push_front((0, 0, 0));

    while deque.len() > 0
    {
        let point = deque.pop_front().unwrap();

        if point.0 < 0 || point.0 >= 22
            || point.1 < 0 || point.1 >= 22
            || point.2 < 0 || point.2 >= 22
        {
            continue;
        }
        if visited[point.0 as usize][point.1 as usize][point.2 as usize] != 0
        {
            continue;
        }
        visited[point.0 as usize][point.1 as usize][point.2 as usize] = 1;

        if check_point(point, blocks)
        {
            continue;
        }

        if check_point(add(point, (1, 0, 0)), blocks) { area += 1; }
        else { deque.push_back(add(point, (1, 0, 0)))}

        if check_point(add(point, (0, 1, 0)), blocks) { area += 1; }
        else { deque.push_back(add(point, (0, 1, 0)))}

        if check_point(add(point, (0, 0, 1)), blocks) { area += 1; }
        else { deque.push_back(add(point, (0, 0, 1)))}


        if check_point(add(point, (-1, 0, 0)), blocks) { area += 1; }
        else { deque.push_back(add(point, (-1, 0, 0)))}

        if check_point(add(point, (0, -1, 0)), blocks) { area += 1; }
        else { deque.push_back(add(point, (0, -1, 0)))}

        if check_point(add(point, (0, 0, -1)), blocks) { area += 1; }
        else { deque.push_back(add(point, (0, 0, -1)))}
    }
    return area;
}


#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 58);
}

fn part_b(content: &'static str) -> usize
{
    let mut blocks: [[[u8; 22]; 22]; 22] = [[[0; 22]; 22]; 22];
    for line in content.lines()
    {
        let values = line.split(',').map(|s| { s.parse::<usize>().unwrap() }).collect::<Vec<usize>>();
        blocks[values[0] + 1][values[1] + 1][values[2] + 1] = 1;
    }

    return calculate_exterior(&blocks);
}



