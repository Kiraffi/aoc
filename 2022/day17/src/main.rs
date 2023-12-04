use std::collections::HashMap;


const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "17";
const DATA: &'static str = include_str!("../../data/day17.txt");
const _TEST_DATA: &'static str = include_str!("../test_data.txt");

const BLOCKS: usize = 5;
const SHAPES: [[&'static str; 4]; BLOCKS] = [
    [
        "....",
        "....",
        "....",
        "####"
    ],

    [
        "....",
        ".#..",
        "###.",
        ".#..",
    ],

    [
        "....",
        "..#.",
        "..#.",
        "###.",
    ],

    [
        "#....",
        "#....",
        "#....",
        "#....",
    ],

    [
        "....",
        "....",
        "##..",
        "##..",
    ],
];

// Rust not allowing for in const fn....
const fn get_bit_row(row: &'static str) -> u32
{
    let mut row_value = 0u32;
    let bytes = row.as_bytes();
    if bytes[0] == '#' as u8 { row_value |= 1; }
    if bytes[1] == '#' as u8 { row_value |= 2; }
    if bytes[2] == '#' as u8 { row_value |= 4; }
    if bytes[3] == '#' as u8 { row_value |= 8; }
    return row_value;
}

const fn get_bit_shape(shape: &[&'static str; 4]) -> u32
{
    let mut block_value = 0u32;
    block_value |= get_bit_row(shape[3]) << (8 * 0 + 2);
    block_value |= get_bit_row(shape[2]) << (8 * 1 + 2);
    block_value |= get_bit_row(shape[1]) << (8 * 2 + 2);
    block_value |= get_bit_row(shape[0]) << (8 * 3 + 2);
    return block_value;
}

const fn get_bit_shapes(shapes: &[[&'static str; 4]; BLOCKS]) -> [u32; BLOCKS]
{
    return [
        get_bit_shape(&shapes[0]),
        get_bit_shape(&shapes[1]),
        get_bit_shape(&shapes[2]),
        get_bit_shape(&shapes[3]),
        get_bit_shape(&shapes[4]),
    ];
}

const BIT_SHAPES: [u32; BLOCKS] = get_bit_shapes(&SHAPES);

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
    println!("Day {}-1: Row height after 2022 blocks: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Row height after 1 000 000 000 000 blocks: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn check_collide(board: &Vec<u8>, block: u32, block_y: i64) -> bool
{
    let coll: u32 = unsafe { *board.as_ptr().offset(block_y as isize ).cast::<u32>() };
    return coll & block != 0;
}

fn add_block(board: &mut Vec<u8>, block: u32, block_y: i64) -> i64
{
    unsafe
    {
        let b32 = board.as_mut_ptr().offset(block_y as isize ).cast::<u32>();
        *b32 |= block;
    }
    let coll: u32 = unsafe { *board.as_ptr().offset(block_y as isize ).cast::<u32>() } & 0x7f7f7f7f;

    let height = (32 - coll.leading_zeros()) / 8;
    return block_y + height as i64 + 1;
}


fn move_block(board: &Vec<u8>, block: u32, block_y: i64, command: i8) -> u32
{
    // Lift block up one row, to not overflow with shifts.
    let new_pos = (block as u64) << (8 + command);

    let collide_walls = (255u64
        // Every 8th bit
        | 0x8080_8080_8080_8080u64
        // Nothing after 5th row
        | (!((1u64 << 40u64) - 1u64))) as u64;

    let collide_blocks: u64 = unsafe { *board.as_ptr().offset((block_y - 1) as isize ).cast::<u64>() };


    // Bottom row
    if new_pos & (collide_blocks | collide_walls) != 0
    {
        return block;
    }
    // Lower it back
    let new_pos = (new_pos >> 8) as u32;
    //let new_pos = new_pos as u32;
    //if check_collide(board, new_pos, block_y)
    //{
    //    return block;
    //}

    return new_pos;
}

fn _move_block_no_check(block: u32, command: i8) -> u32
{
    return block << command;
}


fn _print_line(v: u8)
{
    print!("|");
    for i in 0..7
    {
        print!("{}", if ((v >> i) & 1) != 0 {'#'} else {'.'} );
    }
    //assert_eq!((v >> 7), 0);
    print!("|");
    println!("");
}
fn _print_block(block: u32)
{
    for i in 0..4
    {
        _print_line(((block >> ((3 - i) * 8)) & 255) as u8);
    }
    println!("");
}

fn _print_board(board: &Vec<u8>, from: usize, lines: usize)
{
    println!("from: {} to {}", from, lines + from);
    for i in 0..lines
    {
        _print_line(board[(from + lines) - 1 - i]);
    }
}

fn _print_shapes()
{
    for c in BIT_SHAPES
    {
        for i in 0..4
        {
            _print_line(((c >> ((3 - i) * 8)) & 255) as u8);
        }
        println!("");
    }
}

fn get_new_shape(precalculated: &Vec<Vec<u32>>, block_count: i64, command_count: usize) -> u32
{
    let new_block
        = precalculated[(block_count as usize) % BLOCKS][command_count];
    return new_block;
}

fn get_precalculated_starts(commands: &Vec<i8>) -> Vec<Vec<u32>>
{
    let tmp_board: Vec<u8> = vec![255u8, 0u8, 0u8, 0u8, 0u8, 0u8, 0u8, 0u8, 0u8, 0u8];

    let mut result: Vec<Vec<u32>> = Vec::new();

    for i in 0..BLOCKS
    {
        let mut v: Vec<u32> = Vec::new();

        for j in 0..commands.len()
        {
            let mut block = BIT_SHAPES[i];
            for k in 0..4
            {
                let index = (j + k) % commands.len();
                block = move_block(&tmp_board, block, 1, commands[index]);
            }
            assert!((block & 0x8080_8080u32) == 0);
            v.push(block);
        }
        result.push(v);
    }
    return result;
}

fn parse_commands(content: &'static str) -> Vec<i8>
{
    let mut commands: Vec<i8> = Vec::new();
    for &c in content.as_bytes()
    {
        match c
        {
            60 => commands.push(-1),
            62 => commands.push(1),
            _ => ()
        }
    }
    return commands;
}

fn get_row_count(content: &'static str, block_count_to: i64) -> i64
{
    let commands = parse_commands(content);

    let precalculated = get_precalculated_starts(&commands);
    //_print_shapes();

    let mut board: Vec<u8> = Vec::new();
    board.resize(1024 * 40, 128);
    board[0] = 255;
    let mut block_count = 0;
    let mut command_count = 0;
    let mut row_height = 1i64;
    let mut row_offset = 0i64;
    let mut seen: HashMap<u64, (i64, i64)> = HashMap::new();

    while block_count < block_count_to
    {

        let solid = if row_height > 4
        {
            board[(row_height - 1 - row_offset) as usize]
            | board[(row_height - 2 - row_offset) as usize]
            | board[(row_height - 3 - row_offset) as usize]
            | board[(row_height - 4 - row_offset) as usize]
        }
        else { 0 };

        if solid & 127 == 127
        {
            let bl =
            unsafe { *board.as_ptr().add((row_height - 4 - row_offset) as usize).cast::<u32>() };

            let hash_value = bl as u64
            + ((block_count % BLOCKS as i64) << 32) as u64
            + ((command_count % commands.len()) as u64) << 40;

            if seen.contains_key(&hash_value)
            {
                let last_row = seen[&hash_value];
                let diff_blocks = block_count - last_row.1;
                let diff_height = row_height - last_row.0;
                let multiplier = (block_count_to - block_count) / diff_blocks;
                block_count += multiplier * diff_blocks;
                row_height += multiplier * diff_height;
                *seen.get_mut(&hash_value).unwrap() = (row_height, block_count);
                if block_count >= block_count_to
                {
                    break;
                }
                row_offset += multiplier * diff_height;
            }
            else
            {
                seen.insert(hash_value, (row_height, block_count));
            }
        }
        let mut block_y = row_height + 3 - row_offset - 4;
        let mut new_block = get_new_shape(&precalculated, block_count, command_count % commands.len());
        command_count += 4;

        while block_y > 0 && !check_collide(&board, new_block, block_y)
        {
            new_block = move_block(&board, new_block, block_y, commands[command_count % commands.len()]);
            block_y -= 1;
            command_count += 1;
        }
        row_height = std::cmp::max(row_height, add_block(&mut board, new_block, block_y + 1) + row_offset);

        block_count += 1;
        if row_height - row_offset > 40000
        {
            unsafe
            {
                let mut b1 = board.as_mut_ptr().add(0).cast::<u64>();
                let mut b2 = board.as_ptr().add(20000).cast::<u64>();

                for _ in 0..(board.len() - 20000) / 8
                {
                    *b1 = *b2;
                    b1 = b1.add(1);
                    b2 = b2.add(1);
                }

                for _ in 0..20000 / 8
                {
                    *b1 = 0;
                    b1 = b1.add(1);
                }
            }
            row_offset += 20000;
            board[0] = 255;
        }

        //if row_height % 100_000_000 == 0
        //{
        //    println!("Blocks: {block_count:0>13}");
        //}
    }
    //println!("");
    //println!("command: {} vs commands: {}, commands %: {}", command_count, commands.len(), command_count % commands.len());
    //_print_board(&board,0, 16);
    //_print_board(&board, (row_height - row_offset) as usize - 15, 16);
    //println!("rowoffset: {}", row_offset);
    //println!("thirty {}", thirty);
    //println!("Rowheight: {}", row_height - 1);
    return row_height - 1
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 3068);
}

fn part_a(content: &'static str) -> i64
{
    return get_row_count(content, 2022);
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 1514285714288);
}

fn part_b(content: &'static str) -> i64
{
    return get_row_count(content, 1000000000000i64);

}

