
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "23";
const DATA: &'static str = include_str!("../../data/day23.txt");
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
    println!("Day {}-1: Empty tiles: {}", DAY_STR, part_a(&DATA, 10));
    println!("Day {}-2: Rounds until no one moved: {}", DAY_STR, part_b(&DATA, !0));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn grow_map(map: &mut Vec<Vec<char>>)
{
    for x in 0..map[0].len()
    {
        if map[0][x] == '#'
        {
            map.insert(0, vec!['.'; map[0].len()]);
            break;
        }
    }

    for x in 0..map[map.len() - 1].len()
    {
        if map[map.len() - 1][x] == '#'
        {
            map.push(vec!['.'; map[0].len()]);
            break;
        }
    }

    for y in 0..map.len()
    {
        if map[y][0] == '#'
        {
            for y2 in 0..map.len()
            {
                    map[y2].insert(0, '.');
            }
            break;
        }
    }

    for y in 0..map.len()
    {
        if map[y][map[y].len() - 1] == '#'
        {
            for y2 in 0..map.len()
            {
                    map[y2].push('.');
            }
            break;
        }
    }

}

fn _print_map(map: &Vec<Vec<char>>)
{
    for row in map
    {
        for c in row
        {
            print!("{}", c);
        }
        println!("");
    }
    println!("");
}

fn _print_consider_map(map: &Vec<Vec<u8>>)
{
    for row in map
    {
        for c in row
        {
            print!("{}", c);
        }
        println!("");
    }
    println!("");
}

fn get_consider_pos(map: &Vec<Vec<char>>, x: usize, y: usize, dir: i32) -> (usize, usize)
{
    for i in 0..4
    {
        let tmp_dir = (dir + i) % 4;
        match tmp_dir
        {
            0 => if map[y - 1][x - 1] == '.' && map[y - 1][x + 0] == '.' && map[y - 1][x + 1] == '.'
            {
                return (x, y - 1);
            },
            1 => if map[y + 1][x - 1] == '.' && map[y + 1][x + 0] == '.' && map[y + 1][x + 1] == '.'
            {
                return (x, y + 1);
            },
            2 => if map[y - 1][x - 1] == '.' && map[y + 0][x - 1] == '.' && map[y + 1][x - 1] == '.'
            {
                return (x - 1, y);
            },
            3 => if map[y - 1][x + 1] == '.' && map[y + 0][x + 1] == '.' && map[y + 1][x + 1] == '.'
            {
                return (x + 1, y);
            },
            _ => {}
        }
    }
    return (x, y)
}
fn count_empty(map: &Vec<Vec<char>>) -> usize
{
    let mut result = 0;
    for y in map
    {
        for x in y
        {
            if *x == '.'
            {
                result += 1;
            }
        }
    }
    let mut all_top = true;
    let mut all_bot = true;
    for x in 0..map[0].len()
    {
        all_top &= map[0][x] == '.';
        all_bot &= map[map.len() - 1][x] == '.';
    }
    if all_top { result -= map[0].len(); }
    if all_bot { result -= map[0].len(); }

    let mut all_left = true;
    let mut all_right = true;
    for y in 0..map.len()
    {
        all_left &= map[y][0] == '.';
        all_right &= map[y][map[0].len() - 1] == '.';
    }

    if all_left { result -= map.len(); }
    if all_right { result -= map.len(); }

    if all_left && all_top { result += 1; }
    if all_left && all_bot { result += 1; }
    if all_right && all_top { result += 1; }
    if all_right && all_bot { result += 1; }

    return result;
}

fn simulate_round(map: &mut Vec<Vec<char>>, dir: i32) -> bool
{
    let mut moved = false;
    grow_map(map);
    let mut considers: Vec<Vec<u8>> = vec![vec![0; map[0].len()]; map.len()];
    let mut considerers: Vec<((usize, usize), (usize, usize))> = Vec::new();
    for y in 1..map.len() - 1
    {
        for x in 1..map[0].len() - 1
        {
            if map[y][x] != '#'
            {
                continue;
            }
            let mut elves_near = 0;
            for y1 in y - 1 ..= y + 1
            {
                for x1 in x - 1 ..= x + 1
                {
                    if map[y1][x1] == '#'
                    {
                        elves_near += 1;
                    }
                }
            }
            if elves_near < 2
            {
                continue;
            }
            let consider_pos = get_consider_pos(&map, x, y, dir);
            if x != consider_pos.0 || y != consider_pos.1
            {
                considers[consider_pos.1][consider_pos.0] += 1;
                considerers.push(((x, y), (consider_pos.0, consider_pos.1)));
            }
        }
    }
    //_print_consider_map(&considers);
    for elf in considerers
    {
        if considers[elf.1.1][elf.1.0] == 1
        {
            map[elf.0.1][elf.0.0] = '.';
            map[elf.1.1][elf.1.0] = '#';
            moved = true;
        }
    }
    return moved;
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA, 10);
    assert_eq!(value, 110);
}

fn part_a(content: &'static str, move_rounds_max: usize) -> usize
{
    let mut map = content.lines().map(|x| {x.chars().collect::<Vec<char>>() }).collect::<Vec<Vec<char>>>();
    let mut dir = 0;
    let mut moved = true;
    let mut move_rounds = 0;

    while moved && move_rounds < move_rounds_max
    {
        moved = simulate_round(&mut map, dir);
        dir = (dir + 1) % 4;
        move_rounds += 1;
    }

    return count_empty(&map);
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA, !0);
    assert_eq!(value, 20);
}

fn part_b(content: &'static str, move_rounds_max: usize) -> usize
{
    let mut map = content.lines().map(|x| {x.chars().collect::<Vec<char>>() }).collect::<Vec<Vec<char>>>();
    let mut dir = 0;
    let mut moved = true;
    let mut move_rounds = 0;

    while moved && move_rounds < move_rounds_max
    {
        moved = simulate_round(&mut map, dir);
        dir = (dir + 1) % 4;
        move_rounds += 1;
    }

    return move_rounds;
}



