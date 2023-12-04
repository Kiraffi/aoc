
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "07";

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day07.txt");

    let now = std::time::Instant::now();
    /*
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

fn calculate_size(content: &'static str) -> Vec<usize>
{
    let mut dir_sizes: Vec<usize> = Vec::new();
    let mut amounts: Vec<usize> = Vec::new();

    for line in content.lines()
    {
        if line.starts_with("$ cd /")
        {
            amounts = Vec::new();
            amounts.push(0);
        }
        else if line.starts_with("$ cd ..")
        {
            let amount = amounts.pop().unwrap();
            *amounts.last_mut().unwrap() += amount;
            dir_sizes.push(amount);
        }
        else if line.starts_with("$ cd ")
        {
            amounts.push(0);
        }
        else if !line.starts_with("$ ls") && !line.starts_with("dir") && line.len() > 0
        {
            let word0 = line.split(' ').next().unwrap();
            let file_size = word0.parse::<usize>().unwrap();
            *amounts.last_mut().unwrap() += file_size;
        }
    }
    // Pop didnt work in for loop?
    amounts.reverse();
    let mut amount_sum = 0;
    for amount in amounts
    {
        amount_sum += amount;
        dir_sizes.push(amount_sum);
    }
    return dir_sizes;
}



fn part_a(print_outcome: bool, content: &'static str)
{
    let mut total_under_100k = 0;
    for dir_size in calculate_size(content)
    {
        if dir_size < 100000
        {
            total_under_100k += dir_size;
        }
    }
    if print_outcome
    {
        println!("Day {}-1: Sum of size of directories with size under 100000: {}", DAY_STR, total_under_100k);
    }
}


fn part_b(print_outcome: bool, content: &'static str)
{
    let dir_sizes = calculate_size(content);
    let used_space = *dir_sizes.last().unwrap();

    let mut smallest_folder = !0;
    if 70000000 - used_space >= 30000000
    {
        smallest_folder = 0;
    }
    for dir_size in dir_sizes
    {
        if dir_size >= 30000000 - (70000000 - used_space)
        {
            smallest_folder = std::cmp::min(smallest_folder, dir_size);
        }
    };
    if print_outcome
    {
        println!("Day {}-2: Smallest directory to have over 30000000: {}", DAY_STR, smallest_folder);
    }
}
