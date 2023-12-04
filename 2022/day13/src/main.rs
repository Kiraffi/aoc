use std::cmp::Ordering;


const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "13";
const DATA: &'static str = include_str!("../../data/day13.txt");
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
    println!("Day {}-1: Right orders: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Decoder: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

fn find_right(s: &String, index: usize) -> usize
{
    let bytes = s.as_bytes();
    let mut count = 1;
    for i in index + 1..bytes.len()
    {
        match bytes[i] as char
        {
            '[' => count += 1,
            ']' => count -= 1,
            _ => ()
        }
        if count == 0
        {
            return i;
        }
    }
    for i in index + 1..bytes.len()
    {
        if !(bytes[i] as char).is_ascii_digit()
        {
            return i;
        }
    }
    return bytes.len();
}

fn parse_number(s: &String, i: usize) -> (usize, usize)
{
    let bytes = &s.as_bytes()[i..];
    let mut v: usize = 0;
    let mut i = 0;
    for b in bytes
    {
        let c = *b as char;
        if c.is_ascii_digit()
        {
            v = v * 10 + c as usize - '0' as usize;
        }
        else
        {
            return (v, i);
        }
        i += 1;
    }

    return (v, bytes.len());
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 13);
}

fn parse_inner(mut s1: String, mut s2: String) -> Ordering
{
    let mut i1 = 0;
    let mut i2 = 0;
    loop
    {
        if s2.len() == i2 && s1.len() == i1
        {
            return Ordering::Equal;
        }
        else if s2.len() == i2
        {
            return Ordering::Greater;
        }
        else if s1.len() == i1
        {
            return Ordering::Less;
        }

        let c1 = s1.chars().nth(i1).unwrap();
        let c2 = s2.chars().nth(i2).unwrap();
        if c1 == '[' || c2 == '['
        {
            let mut right_index_1 = find_right(&s1, i1);
            let mut right_index_2 = find_right(&s2, i2);
            if c1 == '[' && c2 != '['
            {
                s2.insert(right_index_2, ']');
                s2.insert(i2, '[');
                right_index_2 += 1;
            }
            else if c1 != '[' && c2 == '['
            {
                s1.insert(right_index_1, ']');
                s1.insert(i1, '[');
                right_index_1 += 1;
            }
            match parse_inner(
                (&s1)[i1 + 1..right_index_1].to_string().clone(),
                (&s2)[i2 + 1..right_index_2].to_string().clone())
            {
                Ordering::Less => return Ordering::Less,
                Ordering::Greater => return Ordering::Greater,
                _ => ()
            }
            i1 = right_index_1;
            i2 = right_index_2;
        }
        else if (c1 == ',' && c2 == ',') || (c1 == ']' && c2 == ']')
        {

        }
        else
        {
            let (v1, len1) = parse_number(&s1, i1);
            let (v2, len2) = parse_number(&s2, i2);
            if v1 < v2
            {
                return Ordering::Less;
            }
            else if v1 > v2
            {
                return Ordering::Greater;
            }
            i1 += len1 - 1;
            i2 += len2 - 1;
        }
        i1 += 1;
        i2 += 1;
    }
}

fn part_a(content: &'static str) -> usize
{
    let mut in_order = 0;
    let mut comp_index = 1;
    for chunk in content.lines().collect::<Vec<&str>>().chunks(3)
    {
        let s1 = chunk[0].to_string();
        let s2 = chunk[1].to_string();

        let points = match parse_inner((&s1[1..s1.len() - 1]).to_string().clone(),
            (&s2[1..s2.len() - 1]).to_string().clone())
        {
            Ordering::Less => comp_index,
            _ => 0
        };

        in_order += points;
        comp_index += 1;

    }

    return in_order;
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 140);
}

fn part_b(content: &'static str) -> usize
{
    let mut comp_index = 1;
    let mut result = 1;
    let mut lines: Vec<&str> = Vec::new();

    for chunk in content.lines().collect::<Vec<&str>>().chunks(3)
    {
        lines.push(chunk[0]);
        lines.push(chunk[1]);
    }
    lines.push("[[2]]");
    lines.push("[[6]]");

    lines.sort_by(|left, right|
        {
            parse_inner((*left).to_string().clone(), right.to_string().clone())
        });

    for s in lines
    {
        result = result * match s
        {
            "[[2]]" => comp_index,
            "[[6]]" => comp_index,
            _ => 1
        };
        comp_index += 1;
    }

    return result;
}



