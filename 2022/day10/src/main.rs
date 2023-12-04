

const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "10";

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day10.txt");

    let now = std::time::Instant::now();
    /*
    for _ in 0..RUN_AMOUNT - 1
    {
        part_a(false, &_data);
        part_b(false, &_data);
    }
    */
    part_a(true, &_data);
    //part_b(true, &_data);
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

struct Cpu
{
    x: i64,
    clock: i64,
    crt: Vec<Vec<u8>>
}

fn draw(cpu: &mut Cpu)
{
    let distance = i64::abs(cpu.x - ((cpu.clock - 1) % 40));
    let c = if distance <= 1 { '#' as u8 } else { '.' as u8 };
    let x = ((cpu.clock - 1) % 40) as usize;
    let y = ((cpu.clock - 1) / 40) as usize;
    cpu.crt[y][x] = c;
}

fn do_cycle(cpu: &mut Cpu, add_value: i64)
{
    draw(cpu);
    cpu.clock += 1;
    cpu.x += add_value;
}

fn eval_signal(cpu: &Cpu) -> i64
{
    if cpu.clock % 40 == 20
    {
        return cpu.clock * cpu.x;
    }
    return 0;
}

fn part_a(print_outcome: bool, content: &'static str)
{
    let mut cpu = Cpu {x: 1, clock: 1, crt: vec![vec!['.' as u8; 40]; 6]};
    let sum_checkpoints = content.lines()
        .fold(0, |prev, line|
        {
            let mut sum = prev;
            let arr: Vec<&str> = line.split(' ').collect();
            let add_value =
                if arr.len() > 1 { arr[1].parse::<i64>().unwrap_or_default() } else { 0 };
            match arr[0]
            {
                "noop" =>
                {
                    do_cycle(&mut cpu, 0);
                    sum += eval_signal(&cpu);
                },
                "addx" =>
                {
                    do_cycle(&mut cpu, 0);
                    sum += eval_signal(&cpu);
                    do_cycle(&mut cpu, add_value);
                    sum += eval_signal(&cpu);
                },
                _ => ()
            }
            return sum;
        });
    if print_outcome
    {
        println!("Day {}-1: Sum of signal points: {}", DAY_STR, sum_checkpoints);
        println!("Day {}-2: Image", DAY_STR);

        for s in cpu.crt
        {
            println!("{}", std::str::from_utf8(s.as_slice()).unwrap());
        }
    }
}

