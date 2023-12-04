
const RUN_AMOUNT:u32 = 1;

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day02.txt");

    let now = std::time::Instant::now();
    for _ in 0..RUN_AMOUNT
    {
        day02_1(true, &_data);
        day02_2(true, &_data);
    }
    println!("Day02 duration: {}us", now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn day02_1(print_outcome: bool, content: &str)
{
    let mut total_score = 0;
    for line in content.lines()
    {
        let opponent_char = line.chars().nth(0).unwrap();
        let you_char = line.chars().nth(2).unwrap();

        let opponent = match opponent_char
        {
            'A' => 0,
            'B' => 1,
            'C' => 2,
            _ => 0
        };
        let you = match you_char
        {
            'X' => 0,
            'Y' => 1,
            'Z' => 2,
            _ => 0
        };
        let diff = (you - opponent + 3) % 3;
        let match_score = match diff
        {
            0 => 3,
            1 => 6,
            2 => 0,
            _ => 0
        };
        let round_score = match_score + you + 1;
        total_score += round_score;

    }

    if print_outcome
    {
        println!("Day 02-1: Points: {}", total_score);
    }
}

fn day02_2(print_outcome: bool, content: &str)
{
    let mut total_score = 0;
    for line in content.lines()
    {
        let opponent_char = line.chars().nth(0).unwrap();
        let you_char = line.chars().nth(2).unwrap();

        let opponent = match opponent_char
        {
            'A' => 0,
            'B' => 1,
            'C' => 2,
            _ => 0
        };
        let (match_score, you) = match you_char
        {
            'X' => (0, (opponent + 2) % 3),
            'Y' => (3, (opponent + 0) % 3),
            'Z' => (6, (opponent + 1) % 3),
            _ => (0, 0)
        };
        let round_score = match_score + you + 1;
        total_score += round_score;
    }

    if print_outcome
    {
        println!("Day 02-2: Points: {}", total_score);
    }
}
