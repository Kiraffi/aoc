use std::collections::HashMap;

const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "21";
const DATA: &'static str = include_str!("../../data/day21.txt");
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
    println!("Day {}-1: Root monkey yells: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Number to yell: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

// Debug for printing
#[derive(Clone, Copy, PartialEq, Debug)]
enum Oper
{
    Add,
    Mul,
    Div,
    Sub,
    Eql,
    Unk,
}

struct Seq
{
    oper: Oper,
    left: usize,
    right: usize,
    result: usize,
    value: i64,
    set_value: bool
}

fn get_or_add_key(names: &mut HashMap<String, usize>, opers: &mut Vec<Seq>, name: &str) -> usize
{
    if names.contains_key(name)
    {
        return names[name];
    }
    let result = opers.len();
    names.insert(name.to_string(), result);
    opers.push(Seq{oper: Oper::Unk, left: !0, right: !0, result: !0, value: !0, set_value: false });
    return result;
}

fn set_seq(seq: &mut Seq, oper: Oper, l: usize, r: usize)
{
    seq.left = l;
    seq.right = r;
    seq.oper = oper;
}

fn parse(content: &'static str) -> (HashMap<String, usize>, Vec<Seq>)
{
    let mut names: HashMap<String, usize> = HashMap::new();
    let mut opers: Vec<Seq> = Vec::new();

    for line in content.lines()
    {
        let (name, oper) = line.split_once(':').unwrap();
        let oper = oper.trim();
        let parts = oper.split(' ').collect::<Vec<&str>>();
        let index = get_or_add_key(&mut names, &mut opers, name);

        if parts.len() == 1
        {
            opers[index].value = parts[0].parse::<i64>().unwrap();
            opers[index].set_value = true;
        }
        else
        {
            let left = get_or_add_key(&mut names, &mut opers, parts[0]);
            let right = get_or_add_key(&mut names, &mut opers, parts[2]);
            opers[left].result = index;

            opers[right].result = index;
            match parts[1]
            {
                "+" => set_seq(&mut opers[index], Oper::Add, left, right),
                "-" => set_seq(&mut opers[index], Oper::Sub, left, right),
                "*" => set_seq(&mut opers[index], Oper::Mul, left, right),
                "/" => set_seq(&mut opers[index], Oper::Div, left, right),
                _ => ()
            }
        }
    }
    return (names, opers)
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 152);
}

fn part_a(content: &'static str) -> i64
{
    let (names, mut opers) = parse(content);

    let mut parsed = false;
    while !parsed
    {
        parsed = true;
        for i in 0..opers.len()
        {
            if opers[i].left == !0 || opers[i].right == !0 || opers[i].set_value
            {
                continue;
            }
            if opers[opers[i].left].set_value && opers[opers[i].right].set_value
            {
                let l_val = opers[opers[i].left].value;
                let r_val = opers[opers[i].right].value;
                opers[i].value = operate(opers[i].oper, l_val, r_val);
                opers[i].set_value = true;
                parsed = false;
            }
        }
    }
    let value = opers[names[&"root".to_string()]].value;
    return value;
}


#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 301);
}

fn operate(oper: Oper, l: i64, r: i64) -> i64
{
    return match oper
    {
        Oper::Add => { l + r },
        Oper::Sub => { l - r },
        Oper::Mul => { l * r },
        Oper::Div => { l / r },
        _ => !0,
    }

}

fn part_b(content: &'static str) -> i64
{

    let (names, mut opers) = parse(content);
    opers[names[&"root".to_string()]].oper = Oper::Eql;
    opers[names[&"humn".to_string()]].oper = Oper::Unk;
    opers[names[&"humn".to_string()]].set_value = false;

    let mut parsed = false;
    while !parsed
    {
        parsed = true;
        for i in 0..opers.len()
        {
            let l_ind = opers[i].left;
            let r_ind = opers[i].right;
            if l_ind == !0 || r_ind == !0
            {
                continue;
            }
            if opers[i].set_value
                && opers[l_ind].set_value
                && opers[r_ind].set_value
            {
                continue;
            }

            if opers[i].set_value
                &&  opers[l_ind].set_value
                && !opers[r_ind].set_value
            {
                let mut l_val = opers[i].value;
                let mut r_val = opers[l_ind].value;

                let oper = match opers[i].oper
                {
                    Oper::Add => Oper::Sub,
                    Oper::Sub => { (l_val, r_val) = (r_val, l_val); Oper::Sub },
                    Oper::Mul => Oper::Div,
                    Oper::Div => { (l_val, r_val) = (r_val, l_val); Oper::Div },
                    _ => opers[r_ind].oper
                };
                opers[r_ind].value = operate(oper, l_val, r_val);
                opers[r_ind].set_value = true;
                parsed = false;
            }
            else if opers[i].set_value
                && !opers[l_ind].set_value
                &&  opers[r_ind].set_value
            {
                let l_val = opers[i].value;
                let r_val = opers[r_ind].value;

                let oper = match opers[i].oper
                {
                    Oper::Add => Oper::Sub,
                    Oper::Sub => Oper::Add,
                    Oper::Mul => Oper::Div,
                    Oper::Div => Oper::Mul,
                    _ => opers[l_ind].oper
                };
                opers[l_ind].value = operate(oper, l_val, r_val);
                opers[l_ind].set_value = true;
                parsed = false;
            }
            else if !opers[i].set_value
                && opers[i].oper != Oper::Eql
                && opers[l_ind].set_value
                && opers[r_ind].set_value
            {
                let l_val = opers[l_ind].value;
                let r_val = opers[r_ind].value;
                opers[i].value = operate(opers[i].oper, l_val, r_val);
                opers[i].set_value = true;
                parsed = false;
            }
            else if !opers[i].set_value
                && opers[i].oper == Oper::Eql
                && opers[l_ind].set_value
            {
                opers[i].value = opers[l_ind].value;
                opers[i].set_value = true;
                opers[r_ind].value = opers[l_ind].value;
                opers[r_ind].set_value = true;
                parsed = false;
            }
            else if !opers[i].set_value
                && opers[i].oper == Oper::Eql
                && opers[r_ind].set_value
            {
                opers[i].value = opers[r_ind].value;
                opers[i].set_value = true;
                opers[l_ind].value = opers[r_ind].value;
                opers[l_ind].set_value = true;
                parsed = false;
            }
        }
    }
    /*
    for (i, p) in opers.iter().enumerate()
    {
        let s = names.iter().for_each(|x| { if *x.1 == i { print!("{} - ", x.0)}});
        println!("{}: {:?} = {}", i, p.oper, p.value);
    }
    */
    let value = opers[names[&"humn".to_string()]].value;
    return value;
}



