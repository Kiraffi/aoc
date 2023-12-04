
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "22";
const DATA: &'static str = include_str!("../../data/day22.txt");
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
    println!("Day {}-1: Password: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Password: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

enum Instruction
{
    Forward(usize),
    Left,
    Right
}

fn parse(content: &'static str) -> (Vec<Vec<char>>, Vec<Instruction>)
{
    let mut map: Vec<Vec<char>> = Vec::new();
    let mut instructions: Vec<Instruction> = Vec::new();

    for line in content.lines()
    {
        if line.starts_with('.') || line.starts_with('#') || line.starts_with(' ')
        {
            map.push(line.chars().collect::<Vec<char>>());
        }
        else if line.len() > 0
        {
            let bytes = line.bytes();
            let mut value = 0;
            for b in bytes
            {
                match b as char
                {
                    'L' => { instructions.push(Instruction::Forward(value)); value = 0; instructions.push(Instruction::Left) },
                    'R' => { instructions.push(Instruction::Forward(value)); value = 0; instructions.push(Instruction::Right) },
                    c => { value = value * 10 + (c as u8 - '0' as u8) as usize }
                }
            }
            instructions.push(Instruction::Forward(value));
        }
    }

    let mut width = map[0].len();
    let height = map.len();

    for i in 0..height
    {
        width = std::cmp::max(width, map[i].len());
    }
    for i in 0..height
    {
        for _ in map[i].len()..width
        {
            map[i].push(' ');
        }
    }


    return (map, instructions);
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 6032);
}

fn part_a(content: &'static str) -> usize
{
    let (map, instructions) = parse(content);
    let height = map.len();
    let width = map[0].len();

    let mut p = (0usize, 0usize);
    if map[0][0] == ' '
    {
        p.0 = map[0].iter().enumerate().find(|i| { *i.1 != ' '}).unwrap().0 as usize
    }
    let mut current_dir = 0usize;
    for ins in instructions
    {
        match ins
        {
            Instruction::Left => current_dir = (current_dir + 3) % 4,
            Instruction::Right =>current_dir = (current_dir + 1) % 4,
            Instruction::Forward(amount) =>
            {
                for _ in 0..amount
                {
                    let mut valid_move = false;
                    let mut next = p;
                    while !valid_move
                    {
                        next.0 = match current_dir
                        {
                            0 => (next.0 + 1) % width,
                            2 => (next.0 + width - 1) % width,
                            _ => next.0
                        };
                        next.1 = match current_dir
                        {
                            1 => (next.1 + 1) % height,
                            3 => (next.1 + height- 1) % height,
                            _ => next.1
                        };
                        match map[next.1][next.0]
                        {
                            ' ' => (),
                            '.' => { valid_move = true; p = next; },
                            _ => { valid_move = true },
                        }
                    }
                }

            }
        }
    }
    return (p.1 + 1) * 1000 + (p.0 + 1) * 4 + current_dir;
}


fn rotate_1_move(map: &mut Vec<Vec<char>>, from: (usize, usize), to: (usize, usize), player: &mut (usize, usize), dir: &mut usize)
{
    let sz = map.len() / 3;
    let mut tmp: Vec<Vec<char>> = Vec::new();
    for x in 0..sz
    {
        let mut tmp2: Vec<char> = Vec::new();
        for y in 0..sz
        {
            let tile = map[from.1 + sz - 1 - y][from.0 + x];
            let tile = match tile
            {
                '^' => '>',
                '>' => 'v',
                'v' => '<',
                '<' => '^',
                c => c
            };
            tmp2.push(tile);
        }
        tmp.push(tmp2);
    }
    for y in 0..sz
    {
        for x in 0..sz
        {
            map[y + from.1][x + from.0] = ' ';
            map[y + to.1][x + to.0] = tmp[y][x];

        }
    }

    if player.0 / sz == from.0 / sz && player.1 / sz == from.1 / sz
    {
        let from_top_left = ((player.0 / sz) * sz, (player.1 / sz) * sz);
        let to_top_left = ((to.0 / sz) * sz, (to.1 / sz) * sz);
        let diff = (player.0 - from_top_left.0, player.1 - from_top_left.1);
        player.0 = to_top_left.0 + sz - 1 - diff.1;
        player.1 = to_top_left.1 + diff.0;
        *dir = (*dir + 1) % 4;
    }
}

fn rotate_2_move(map: &mut Vec<Vec<char>>, from: (usize, usize), to: (usize, usize), player: &mut (usize, usize), dir: &mut usize)
{
    rotate_1_move(map, from, to, player, dir);
    rotate_1_move(map, to, to, player, dir);
}

fn rotate_3_move(map: &mut Vec<Vec<char>>, from: (usize, usize), to: (usize, usize), player: &mut (usize, usize), dir: &mut usize)
{
    rotate_2_move(map, from, to, player, dir);
    rotate_1_move(map, to, to, player, dir);
}

fn _print_map(map: &Vec<Vec<char>>)
{
    for line in map
    {
        for c in line
        {
            print!("{}", *c);
        }
        println!("");
    }
    println!("");
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 5031);
}

fn part_b(content: &'static str) -> usize
{
    let (mut map, instructions) = parse(content);
    let mut height = map.len();
    let mut width = map[0].len();

    let mut p = (0usize, 0usize);
    if map[0][0] == ' '
    {
        p.0 = map[0].iter().enumerate().find(|i| { *i.1 != ' '}).unwrap().0 as usize
    }
    let mut current_dir = 0;
    let rotated = width < height;
    if rotated
    {
        let mut tmp: Vec<Vec<char>> = Vec::new();
        for x in 0..width
        {
            let mut tmp2: Vec<char> = Vec::new();
            for y in 0..height
            {
                tmp2.push(map[height - 1 - y][x]);
            }
            tmp.push(tmp2);
        }
        map = tmp;
        current_dir = 1;
        p = (height - p.1 - 1, p.0);
    }
    height = map.len();
    width = map[0].len();

    let sz = width / 4;

    // From, to rotation
    let mut inverse_rotate_list: Vec<((usize, usize), (usize, usize), usize)> = Vec::new();
    {
        // Check left middle
        if map[sz][0] == ' ' && map[0][0] != ' '
        {
            rotate_3_move(&mut map, (0, 0), (0, sz), &mut p, &mut current_dir);
            inverse_rotate_list.push(((0, sz), (0, 0), 1));
        }
        else if map[sz][0] == ' ' && map[sz * 2][0] != ' '
        {
            rotate_1_move(&mut map, (0, sz * 2), (0, sz), &mut p, &mut current_dir);
            inverse_rotate_list.push(((0, sz), (0, sz * 2), 3));
        }

        // Check right middle
        if map[sz][sz * 3] == ' ' && map[0][sz * 3] != ' '
        {
            rotate_1_move(&mut map, (sz * 3, 0), (sz * 3, sz), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz * 3, sz), (sz * 3, 0), 3));
        }
        else if map[sz][sz * 3] == ' ' && map[sz * 2][sz * 3] != ' '
        {
            rotate_3_move(&mut map, (sz * 3, sz * 2), (sz * 3, sz), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz * 3, sz), (sz * 3, sz * 2), 1));
        }

        // Check top
        if map[0][sz] == ' ' && map[0][0] != ' '
        {
            rotate_1_move(&mut map, (0, 0), (sz, 0), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz, 0), (0, 0), 3));
        }
        else if map[0][sz] == ' ' && map[0][sz * 2] != ' '
        {
            rotate_3_move(&mut map, (sz * 2, 0), (sz, 0), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz, 0), (sz * 2, 0), 1));
        }
        else if map[0][sz] == ' ' && map[0][sz * 3] != ' '
        {
            rotate_2_move(&mut map, (sz * 3, 0), (sz, 0), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz, 0), (sz * 3, 0), 2));
        }

        // Check bottom
        if map[sz * 2][sz * 2] == ' ' && map[sz * 2][0] != ' '
        {
            rotate_2_move(&mut map, (0, sz * 2), (sz * 2, sz * 2), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz * 2, sz * 2), (0, sz * 2), 2));
        }
        else if map[sz * 2][sz * 2] == ' ' && map[sz * 2][sz * 2] != ' '
        {
            rotate_3_move(&mut map, (sz * 2, sz * 2), (sz * 2, sz * 2), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz * 2, sz * 2), (sz * 2, sz * 2), 1));
        }
        else if map[sz * 2][sz * 2] == ' ' && map[sz * 2][sz * 3] != ' '
        {
            rotate_1_move(&mut map, (sz * 3, sz * 2), (sz * 2, sz * 2), &mut p, &mut current_dir);
            inverse_rotate_list.push(((sz * 2, sz * 2), (sz * 3, sz * 2), 3));
        }

    }
    for ins in instructions
    {
        match ins
        {
            Instruction::Left =>  current_dir = (current_dir + 3) % 4,
            Instruction::Right => current_dir = (current_dir + 1) % 4,
            Instruction::Forward(amount) =>
            {
                for _ in 0..amount
                {
                    let mut valid_move = false;
                    let mut next = p;
                    let mut saved_rot = current_dir % 4;
                    next.0 = match saved_rot
                    {
                        0 => (next.0 + 1) % width,
                        2 => (next.0 + width - 1) % width,
                        _ => next.0
                    };
                    next.1 = match saved_rot
                    {
                        1 => (next.1 + 1) % height,
                        3 => (next.1 + height- 1) % height,
                        _ => next.1
                    };
                    while !valid_move
                    {
                        match map[next.1][next.0]
                        {
                            ' ' =>
                            {
                                // Top to bottom wrap
                                if next.1 == height - 1 && saved_rot == 3
                                {
                                    next.0 = 4 * sz - 1 - next.0 % sz;
                                    next.1 = sz;
                                    saved_rot = (saved_rot + 2) % 4;
                                }
                                else if next.0 >= sz * 3 && next.1 == sz - 1 && saved_rot == 3
                                {
                                    next.0 = 2 * sz - 1 - next.0 % sz;
                                    next.1 = 0;
                                    saved_rot = (saved_rot + 2) % 4;
                                }

                                // Top left
                                else if next.0 == sz - 1 && saved_rot == 2
                                {
                                    next.0 = next.1;
                                    next.1 = sz;
                                    saved_rot = (saved_rot + 3) % 4;
                                }
                                else if next.0 < sz && next.1 == sz - 1 && saved_rot == 3
                                {
                                    next.1 = next.0;
                                    next.0 = sz;
                                    saved_rot = (saved_rot + 1) % 4;
                                }

                                // Top right
                                else if next.0 == 2 * sz && saved_rot == 0
                                {
                                    next.0 = sz * 3 - 1 - next.1;
                                    next.1 = sz;
                                    saved_rot = (saved_rot + 1) % 4;
                                }
                                else if next.0 >= 2 * sz && next.0 < 3 * sz && next.1 == sz - 1 && saved_rot == 3
                                {
                                    next.1 = sz - 1 - next.0 % sz;
                                    next.0 = sz * 2 - 1;
                                    saved_rot = (saved_rot + 3) % 4;
                                }


                                // From bottom bottom
                                else if next.1 == 0 && saved_rot == 1
                                {
                                    next.0 = sz - next.0 % sz - 1;
                                    next.1 = 2 * sz - 1;
                                    saved_rot = (saved_rot + 2) % 4;
                                }
                                else if next.0 < sz && next.1 == 2 * sz && saved_rot == 1
                                {
                                    next.0 = 3 * sz - 1 - next.0;
                                    next.1 = height - 1;
                                    saved_rot = (saved_rot + 2) % 4;

                                }

                                // Bot left
                                else if next.0 == 2 * sz - 1 && saved_rot == 2
                                {
                                    next.0 = sz * 2 - 1 - next.1 % sz;
                                    next.1 = sz * 2 - 1;
                                    saved_rot = (saved_rot + 1) % 4;
                                }
                                else if next.0 >= sz && next.0 < 2 * sz && next.1 == 2 * sz && saved_rot == 1
                                {
                                    next.1 = sz * 3 - 1 - next.0 % sz;
                                    next.0 = 2 * sz;
                                    saved_rot = (saved_rot + 3) % 4;
                                }

                                // Bot right
                                else if next.0 == 3 * sz && saved_rot == 0
                                {
                                    next.0 = sz * 3 + next.1 % sz;
                                    next.1 = sz * 2 - 1;
                                    saved_rot = (saved_rot + 3) % 4;
                                }
                                else if next.0 >= 3 * sz && next.1 == 2 * sz && saved_rot == 1
                                {
                                    next.1 = 2 * sz + next.0 % sz;
                                    next.0 = 3 * sz - 1;
                                    saved_rot = (saved_rot + 1) % 4;
                                }
                            },
                            '#' => { valid_move = true },
                            _ => { valid_move = true; p = next; current_dir = saved_rot },
                        }
                    }
                    current_dir = current_dir % 4;
                    map[p.1][p.0] = match current_dir
                    {
                        0 => '>',
                        1 => 'v',
                        2 => '<',
                        3 => '^',
                        _ => '.'
                    };
                    //println!("Move: {}", ins_index);
                    //_print_map(&map);
                }

            }
        }
    }
    //_print_map(&map);
    //println!("dir before inverse rot move: {}", current_dir);
    while inverse_rotate_list.len() > 0
    {
        let last = inverse_rotate_list.pop().unwrap();
        match last.2
        {
            1 => rotate_1_move(&mut map, last.0, last.1, &mut p, &mut current_dir),
            2 => rotate_2_move(&mut map, last.0, last.1, &mut p, &mut current_dir),
            3 => rotate_3_move(&mut map, last.0, last.1, &mut p, &mut current_dir),
            _ => {}
        }
    }
    //_print_map(&map);
    //println!("dir after inverse rot move: {}", current_dir);

    // Reverse the original rotation if height > width
    if rotated
    {
        let mut tmp: Vec<Vec<char>> = Vec::new();
        for x in 0..width
        {
            let mut tmp2: Vec<char> = Vec::new();
            for y in 0..height
            {
                let tile = map[y][width - 1 - x];
                let tile = match tile
                {
                    '^' => '<',
                    '>' => '^',
                    'v' => '>',
                    '<' => 'v',
                    c => c
                };
                tmp2.push(tile);
            }
            tmp.push(tmp2);
        }
        map = tmp;

        current_dir = (current_dir + 3) % 4;
        p = (p.1, width - p.0 - 1);
    }
    //println!("dir after rotate: {}", current_dir);
    //println!("posx: {}, posy: {}", p.0, p.1);
    //_print_map(&map);
    return (p.1 + 1) * 1000 + (p.0 + 1) * 4 + current_dir;
}



