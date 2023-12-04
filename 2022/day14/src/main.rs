
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "14";
const DATA: &'static str = include_str!("../../data/day14.txt");
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
    println!("Day {}-1: Units fallen before falling to pit: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Needs to drop: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}


#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 24);
}

#[derive(Clone)]
struct Point
{
    x: usize,
    y: usize
}

impl Point
{
    fn min(a: &Point, b: &Point) -> Point
    {
        Point{x: std::cmp::min(a.x, b.x), y: std::cmp::min(a.y, b.y)}
    }
    fn max(a: &Point, b: &Point) -> Point
    {
        Point{x: std::cmp::max(a.x, b.x), y: std::cmp::max(a.y, b.y)}
    }
}

fn get_sorted(p1: &Point, p2: &Point) -> (Point, Point)
{
    return (Point::min(&p1, &p2), Point::max(&p1, &p2));
}
fn create_map(min: &Point, max: &Point) -> Vec<Vec<char>>
{
    let mut map: Vec<Vec<char>> = Vec::new();
    map.resize_with(max.y - min.y + 4,
        ||{ let mut v = Vec::new(); v.resize(max.x - min.x + 100, ' '); v } );

    return map;
}

fn read_content(content: &str) -> (Vec<Vec<Point>>, Point, Point)
{
    let mut max = Point {x: 500, y: 0 };
    let mut min = Point {x: 500, y: 0};
    let mut points: Vec<Vec<Point>> = Vec::new();
    for lines in content.lines()
    {
        points.push(Vec::new());
        for pair in lines.split(" -> ")
        {
            let values =
                pair.split(',').map(|v| { v.parse::<usize>().unwrap() }).collect::<Vec<usize>>();
            let new_point = Point{x: values[0], y: values[1]};
            min = Point::min(&min, &new_point);
            max = Point::max(&max, &new_point);
            points.last_mut().unwrap().push(new_point);
        }
    }

    return (points, min, max);
}
fn draw_map(map: &mut Vec<Vec<char>>, points: &mut Vec<Vec<Point>>, min: &Point)
{
    for line in points.iter_mut()
    {
        let mut last = line[0].clone();
        last.x = last.x - min.x + 1;
        last.y = last.y - min.y;
        for p in (*line).iter_mut()
        {
            p.x = p.x - min.x + 1;
            p.y = p.y - min.y;
            let (a, b) = get_sorted(&p, &last);
            for y in a.y..=b.y
            {
                for x in a.x..=b.x
                {
                    map[y][x] = '#';
                }
            }
            last = p.clone();
        }
    }
}

fn simulate(map: &mut Vec<Vec<char>>, min: &Point, max: &Point) -> usize
{
    let max = Point{x: max.x - min.x, y: max.y - min.y };
    let mut sands = 0;
    'outer: loop
    {
        let mut sand = Point{x: 501 - min.x, y: 0};
        loop
        {
            if sand.x < 1 || sand.x > max.x + 1 || sand.y > max.y
            {
                break 'outer;
            }

            if map[sand.y + 1][sand.x] == ' '
            {
                sand.y += 1;
            }
            else if map[sand.y + 1][sand.x - 1] == ' '
            {
                sand.y += 1;
                sand.x -= 1;
            }
            else if map[sand.y + 1][sand.x + 1] == ' '
            {
                sand.y += 1;
                sand.x += 1;
            }
            else if sand.x == 501 - min.x && sand.y == 0
            {
                sands += 1;
                break 'outer;
            }
            else
            {
                map[sand.y][sand.x] = '+';
                break;
            }
        }
        sands += 1;
    }

    //println!("min {}:{}, max {}: {}", min.x, min.y, max.x, max.y);
    return sands;

}

fn part_a(content: &'static str) -> usize
{
    let (mut points, min, max) = read_content(content);

    let mut map = create_map(&min, &max);
    draw_map(&mut map, &mut points, &min);

    //for y in 0..map.len()
    //{
    //    println!("{}", String::from_iter(map[y].iter()));
    //}
    return simulate(&mut map, &min, &max);
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 93);
}

fn part_b(content: &'static str) -> usize
{
    let (mut points, mut min, mut max) = read_content(content);
    points.push(Vec::new());
    points.last_mut().unwrap().push(Point{x: 10, y: max.y + 2});
    points.last_mut().unwrap().push(Point{x: 1000, y: max.y + 2});
    min.x = 10;
    max.x = 1000;
    max.y += 2;
    let mut map = create_map(&min, &max);
    draw_map(&mut map, &mut points, &min);

    //for y in 0..map.len()
    //{
    //    println!("{}", String::from_iter(map[y].iter()));
    //}
    return simulate(&mut map, &min, &max);
}



