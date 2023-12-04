
use std::collections::HashSet;

const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "09";

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day09.txt");

    let now = std::time::Instant::now();
    /*    let mut top_visible = true;
    let mut bot_visible = true;

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

#[derive(Clone, Copy, Eq, Hash, PartialEq)]
struct Pos
{
    x: i16,
    y: i16
}

fn simulate_movement(content: &'static str, knots: usize) -> usize
{
    let mut rope: Vec<Pos> = Vec::new();
    rope.resize(knots, Pos{x: 0, y: 0});
    let mut visited: HashSet<Pos> = HashSet::new();
    for line in content.lines()
    {
        let (dir, value) = line.split_once(' ').unwrap();
        let amount = value.parse::<i32>().unwrap();
        let dir = dir.as_bytes()[0] as char;
        for _ in 0..amount
        {
            match dir
            {
                'L' => rope[0].x -= 1,
                'R' => rope[0].x += 1,
                'U' => rope[0].y -= 1,
                'D' => rope[0].y += 1,
                _ => ()
            }
            for i in 1..knots
            {
                let prev = rope[i - 1];
                let mut curr = &mut rope[i];
                let diff_x = prev.x - curr.x;
                let diff_y = prev.y - curr.y;
                let abs_x = i16::abs(diff_x);
                let abs_y = i16::abs(diff_y);

                if (abs_x | abs_y) >= 2
                {
                    if diff_x < 0 { curr.x -= 1; }
                    if diff_x > 0 { curr.x += 1; }
                    if diff_y < 0 { curr.y -= 1; }
                    if diff_y > 0 { curr.y += 1; }
                }
            }
            let &tail = &rope[knots - 1];
            visited.insert(tail);
        }
    }
    return visited.len();
}

fn part_a(print_outcome: bool, content: &'static str)
{
    let visited_tiles = simulate_movement(content, 2);
    if print_outcome
    {
        println!("Day {}-1: Tail visiting tile amount: {}", DAY_STR, visited_tiles);
    }
}

fn part_b(print_outcome: bool, content: &'static str)
{
    let visited_tiles = simulate_movement(content, 10);
    if print_outcome
    {
        println!("Day {}-2: Tail visited tile amount: {}", DAY_STR, visited_tiles);
    }
}
