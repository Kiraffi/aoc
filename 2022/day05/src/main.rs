const RUN_AMOUNT:u32 = 1;

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day05.txt");

    let now = std::time::Instant::now();
    /*
    for _ in 0..RUN_AMOUNT - 1
    {
        day05_1(false, &_data);
        day05_2(false, &_data);
    }
    */
    day05_1(true, &_data);
    day05_2(true, &_data);
    println!("Day05 duration: {}us", now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

struct Command
{
    amount: usize,
    from: usize,
    to: usize
}

fn parse_line(line: &str, crates: &mut Vec<Vec<u8>>, commands: &mut Vec<Command>)
{
    if line.contains('[')
    {
        let bytes = line.as_bytes();
        let crate_amount = line.len() / 4 + 1;
        for i in 0..crate_amount
        {
            while crates.len() <= i
            {
                crates.push(Vec::new());
            }
            let value = bytes[i * 4 + 1];
            if value != ' ' as u8
            {
                crates[i].insert(0, value);
            }
        }
    }
    else if line.contains('m')
    {
        let mut splits = line.split(' ');
        let mut command = Command {amount: 0, from: 0, to: 0};
        let _ = splits.next();
        command.amount = splits.next().unwrap().parse::<usize>().unwrap();
        let _ = splits.next();
        command.from = splits.next().unwrap().parse::<usize>().unwrap();
        let _ = splits.next();
        command.to = splits.next().unwrap().parse::<usize>().unwrap();
        commands.push(command);
    }
}

fn day05_1(print_outcome: bool, content: &str)
{
    let mut crates: Vec<Vec<u8>> = Vec::new();
    let mut commands: Vec<Command> = Vec::new();
    let lines = content.lines();
    for line in lines
    {
        parse_line(line, &mut crates, &mut commands);
    }

    for c in commands
    {
        let from_len = crates[c.from - 1].len() - c.amount;
        let mut slice: Vec<u8> = Vec::new();
        slice.extend_from_slice(&crates[c.from - 1][from_len..]);
        slice.reverse();
        crates[c.from - 1].truncate(from_len);
        crates[c.to - 1].extend_from_slice(&slice);

    }

    if print_outcome
    {
        print!("Day 05-1: ");
        for c in crates
        {
            print!("{}", c[c.len() - 1] as char);
        }
        println!("");
    }
}

fn day05_2(print_outcome: bool, content: &str)
{
    let mut crates: Vec<Vec<u8>> = Vec::new();
    let mut commands: Vec<Command> = Vec::new();
    let lines = content.lines();
    for line in lines
    {
        parse_line(line, &mut crates, &mut commands);
    }

    for c in commands
    {
        let from_len = crates[c.from - 1].len() - c.amount;
        let mut slice: Vec<u8> = Vec::new();
        slice.extend_from_slice(&crates[c.from - 1][from_len..]);
        crates[c.from - 1].truncate(from_len);
        crates[c.to - 1].extend_from_slice(&slice);
    }

    if print_outcome
    {
        print!("Day 05-2: ");
        for c in crates
        {
            print!("{}", c[c.len() - 1] as char);
        }
        println!("");
    }
}
