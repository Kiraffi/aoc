use std::collections::{HashMap, VecDeque};
use std::sync::atomic::{Ordering, AtomicI64};

const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "16";
const DATA: &'static str = include_str!("../../data/day16.txt");
const _TEST_DATA: &'static str = include_str!("../test_data.txt");


static MAX_VALUE: AtomicI64 = AtomicI64::new(0);

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
    println!("Day {}-1: Most pressure released: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Most pressure released with elephant: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}


struct Connection
{
    rooms: Vec<String>,
    flow: i64,
}

struct Path
{
    end: String,
    distance: i64,
    flow: i64
}

#[derive(Clone)]
struct State
{
    pos: String,
    flow_total: i64,
    time: i64
}

fn get_flow(connections: &HashMap<&str, Vec<Path>>,
    states: Vec<State>, visited: Vec<String>, flow_total: i64, max_time: i64) -> i64
{
    let mut index = 0;
    for i in 0..states.len()
    {
        if states[i].time < states[index].time
        {
            index = i;
        }
    }
    let curr = &states[index];
    let mut flow_total_max = flow_total;
    let mut max_max = flow_total;
    for conn in &connections[&curr.pos[0..]]
    {
        if !visited.iter().find(|item| { *item == &conn.end }).is_none()
        {
            continue;
        }
        let new_time = curr.time + conn.distance + 1;
        max_max += conn.flow * std::cmp::max(0, max_time - new_time);
    }
    if max_max < MAX_VALUE.load(Ordering::Acquire)
    {
        return 0;
    }
    for conn in &connections[&curr.pos[0..]]
    {
        if !visited.iter().find(|item| { *item == &conn.end }).is_none()
        {
            continue;
        }
        let new_time = curr.time + conn.distance + 1;
        if new_time >= max_time
        {
            continue;
        }
        let new_flow_total = flow_total + (max_time - new_time) * conn.flow;
        let mut new_state = states.clone();
        new_state[index] = State{pos: conn.end.clone(),
            flow_total: curr.flow_total + (max_time - new_time) * conn.flow,
            time: new_time };
        let mut new_visited = visited.clone();

        new_visited.push(conn.end.clone());

        let flow_val = get_flow(connections, new_state, new_visited, new_flow_total, max_time);
        flow_total_max = std::cmp::max(flow_total_max, flow_val);
    }
    _ = MAX_VALUE.fetch_max(flow_total_max, Ordering::AcqRel);

    return flow_total_max;
}

fn find_distances(name: &str, connections: &HashMap<&str, Connection>) -> Vec<Path>
{
    let mut result: Vec<Path> = Vec::new();
    let mut posses: VecDeque<(&str, i64)> = VecDeque::new();
    let mut visited: HashMap<&str, i64> = HashMap::new();
    posses.push_front((name, 0));
    visited.insert(name, 0);
    while posses.len() > 0
    {
        let (pos, distance) = posses.pop_front().unwrap();
        if (connections[pos].flow > 0 || pos == "AA") && name != pos
        {
            result.push(Path{end: pos.to_string(), distance, flow: connections[pos].flow});
        }
        for room in &connections[pos].rooms
        {
            if visited.contains_key(&room[0..])
            {
                continue;
            }
            posses.push_back((room, distance + 1));
            visited.insert(room, distance + 1);
        }
    }
    return result;
}

fn parse(content: &str) -> HashMap<&str, Vec<Path>>
{
    let mut connections: HashMap<&str, Connection> = HashMap::new();
    for lines in content.lines()
    {
        let words: Vec<&str> = lines.split(' ').collect();
        let mut rooms: Vec<String> = Vec::new();
        let flow = words[4].split_once('=').unwrap()
            .1.split_once(';').unwrap_or((words[4], ""))
            .0.parse::<i64>().unwrap();
        for i in 9..words.len()
        {
            rooms.push(words[i].split_once(',').unwrap_or((words[i], ""))
                .0.to_string());
        }
        connections.insert(words[1], Connection { rooms: rooms.clone(), flow: flow });

    }
    let mut paths: HashMap<&str, Vec<Path>> = HashMap::new();
    for conn in &connections
    {
        if conn.1.flow > 0 || *conn.0 == "AA"
        {
            let res = find_distances(conn.0, &connections);
            paths.insert(conn.0, res);
        }
    }
    return paths;
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 1651);
}


fn part_a(content: &'static str) -> i64
{
    MAX_VALUE.store(0, Ordering::Release);
    let paths = parse(content);
    let mut states: Vec<State> = Vec::new();
    states.push(State {pos: "AA".to_string(), flow_total: 0, time: 0});
    let max = get_flow(&paths, states, Vec::new(), 0, 30);

    return max;
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 1707);
}

fn part_b(content: &'static str) -> i64
{
    MAX_VALUE.store(0, Ordering::Release);
    let paths = parse(content);
    let mut states: Vec<State> = Vec::new();
    states.push(State {pos: "AA".to_string(), flow_total: 0, time: 0});
    states.push(State {pos: "AA".to_string(), flow_total: 0, time: 0});
    let max = get_flow(&paths, states, Vec::new(), 0, 26);

    return max;
}



