
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "25";
const DATA: &'static str = include_str!("../../data/day25.txt");
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
    println!("Day {}-1: Snafu number: {}", DAY_STR, part_a(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn pow5(a: i64) -> i64
{
    return (0..a).fold(1, |p, _|{ p * 5 });
}

fn to_snafu_number(mut a: i64) -> String
{
    let mut s = String::new();

    a -= 1;
    while a > 0
    {
        let c = match a % 5
        {
            0 => '1',
            1 => '2',
            2 => '=',
            3 => '-',
            4 => '0',
            _ => unreachable!()
        };
        s.insert(0, c);
        a -= 2;
        a /= 5;
    }
    return s;
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, "2=-1=0");
}

fn part_a(content: &'static str) -> String
{
    let mut sum = 0;
    for line in content.lines()
    {
        let bytes = line.as_bytes();
        for i in 0..bytes.len()
        {
            let num = match bytes[bytes.len() - 1 - i] as char
            {
                '2' => 2,
                '1' => 1,
                '0' => 0,
                '-' => -1,
                '=' => -2,
                _ => 0
            };
            sum += pow5(i as i64) * num;
        }
    }

    return to_snafu_number(sum);
}

