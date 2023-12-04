use std::mem::swap;

fn main()
{
    const RUN_AMOUNT:u32 = 1;
    let data = include_str!("../../data/day01.txt");
    let now = std::time::Instant::now();
    for _ in 0..RUN_AMOUNT
    {
        day01(true, &data);
    }
    println!("Day01 duration: {}us", now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn day01(print_outcome: bool, content: &str)
{
    const NUMBER_AMOUNT:usize = 3;
    let mut max_amounts: [i32; NUMBER_AMOUNT] = [0; NUMBER_AMOUNT];
    let mut current_amount = 0;

    for line in content.lines()
    {
        if line.len() != 0
        {
            current_amount += line.parse::<i32>().unwrap();
        }
        else
        {
            for n in 0..NUMBER_AMOUNT
            {
                if max_amounts[n] < current_amount
                {
                    swap(&mut max_amounts[n], &mut current_amount);
                }
            }
            current_amount = 0;
        }

    }

    let mut sum_of_maxes = 0;
    for value in max_amounts
    {
        sum_of_maxes += value;
    }

    if print_outcome
    {
        println!("Day 01-1: Highest: {}", max_amounts[0]);
        println!("Day 01-2: Sum of highest {} values: {}", NUMBER_AMOUNT, sum_of_maxes);
    }
}
