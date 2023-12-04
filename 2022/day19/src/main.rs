
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "19";
const DATA: &'static str = include_str!("../../data/day19.txt");
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
    println!("Day {}-1: Qualities: {}", DAY_STR, part_a(&DATA));
    println!("Day {}-2: Geodes: {}", DAY_STR, part_b(&DATA));
    println!("Day {} duration: {}us", DAY_STR, now.elapsed().as_micros() as f32 / RUN_AMOUNT as f32);
}

struct Blueprint
{
    ore_robot: i64,
    clay_robot: i64,
    obsidian_robot: (i64, i64),
    geode_robot: (i64, i64),
    max_ore_needed: i64
}

#[derive(Clone)]
enum Build
{
    Ore,
    Clay,
    Obsidian,
    Geode,
    None
}

#[derive(Clone)]
struct State
{
    index: i32,
    build: Build,
    time: i64,

    ore: i64,
    clay: i64,
    obsidian: i64,
    geode: i64,

    ore_robots: i64,
    clay_robots: i64,
    obsidian_robots: i64,
    geode_robots: i64
}

impl State
{
    fn new() -> State
    {
        State {
            index: 0,
            build: Build::None,
            time: 0,
            ore: 0,
            clay: 0,
            obsidian: 0,
            geode: 0,
            ore_robots: 1,
            clay_robots: 0,
            obsidian_robots: 0,
            geode_robots: 0
        }
    }
}

fn build_turn_needed(blueprint: &Blueprint, state: &State) -> i64
{
    return match state.build
    {
        Build::Ore => {
            round_up_div(blueprint.ore_robot - state.ore, state.ore_robots)
        },
        Build::Clay => {
            round_up_div(blueprint.clay_robot - state.ore, state.ore_robots)
        },
        Build::Obsidian => {
            let ore_turns = round_up_div(blueprint.obsidian_robot.0 - state.ore, state.ore_robots);
            let clay_turns = round_up_div(blueprint.obsidian_robot.1 - state.clay, state.clay_robots);
            std::cmp::max(ore_turns, clay_turns)
        },
        Build::Geode => {
            let ore_turns = round_up_div(blueprint.geode_robot.0 - state.ore, state.ore_robots);
            let obsidian_turns = round_up_div(blueprint.geode_robot.1 - state.obsidian, state.obsidian_robots);
            std::cmp::max(ore_turns, obsidian_turns)
        },
        Build::None => 0
    }
}

fn build(blueprint: &Blueprint, state: &mut State)
{
    match state.build
    {
        Build::Ore => { state.ore_robots += 1; state.ore -= blueprint.ore_robot },
        Build::Clay => { state.clay_robots += 1; state.ore -= blueprint.clay_robot },
        Build::Obsidian => { state.obsidian_robots += 1; state.ore -= blueprint.obsidian_robot.0; state.clay -= blueprint.obsidian_robot.1; },
        Build::Geode => { state.geode_robots += 1; state.ore -= blueprint.geode_robot.0; state.obsidian -= blueprint.geode_robot.1; },
        Build::None => ()
    }
}

fn round_up_div(value: i64, divider: i64) -> i64
{
    return (value + divider - 1) / divider;
}
fn collect(state: &mut State, turns: i64)
{
    state.time += turns;
    state.ore += state.ore_robots * turns;
    state.clay += state.clay_robots * turns;
    state.obsidian += state.obsidian_robots * turns;
    state.geode += state.geode_robots * turns;
}

fn solve_recursive(blueprint: &Blueprint, mut state: State, max_time: i64, current_best: &mut Vec<i64>) -> i64
{
    let build_turns = std::cmp::max(1, build_turn_needed(blueprint, &state) + 1);
    if state.time + build_turns >= max_time
    {
        let max_score = state.geode + (max_time - state.time) * state.geode_robots;
        current_best[(max_time - 1) as usize] = std::cmp::max(max_score, current_best[(max_time - 1) as usize]);
        return max_score;
    }
    collect(&mut state, build_turns);
    build(blueprint, &mut state);

    let time_left = max_time - state.time;
    let mut max_score = state.geode + time_left * state.geode_robots;
    let potential = (time_left + 1 - 1) * (time_left + 0 - 1) / 2;

    if potential + max_score < current_best[state.time as usize]
    {
        return max_score;
    }

    let mut new_state = state.clone();
    new_state.index += 1;
    if state.obsidian_robots > 1
    {
        let mut new_state = state.clone();
        new_state.index += 1;
        new_state.build = Build::Geode;
        max_score = std::cmp::max(max_score, solve_recursive(blueprint, new_state, max_time, current_best));
    }
    if state.clay_robots > 1 && state.obsidian_robots < blueprint.geode_robot.1
    {
        let mut new_state = state.clone();
        new_state.index += 1;
        new_state.build = Build::Obsidian;
        max_score = std::cmp::max(max_score, solve_recursive(blueprint, new_state, max_time, current_best));
    }

    if state.clay_robots < blueprint.obsidian_robot.1
    {
        let mut new_state = state.clone();
        new_state.index += 1;
        new_state.build = Build::Clay;
        max_score = std::cmp::max(max_score, solve_recursive(blueprint, new_state, max_time, current_best));
    }
    if state.ore_robots < blueprint.max_ore_needed
    {
        let mut new_state = state.clone();
        new_state.index += 1;
        new_state.build = Build::Ore;
        max_score = std::cmp::max(max_score, solve_recursive(blueprint, new_state, max_time, current_best));
    }
    current_best[state.time as usize] = std::cmp::max(max_score, current_best[state.time as usize]);

    return max_score;
}

#[test]
fn part_a_test()
{
    let value = part_a(&_TEST_DATA);
    assert_eq!(value, 33);
}

fn parse_blueprints(content: &'static str) -> Vec<Blueprint>
{
    let mut blueprints: Vec<Blueprint> = Vec::new();
    for line in content.lines()
    {
        let mut numbers: Vec<i64> = Vec::new();
        for w in line.split(' ')
        {
            let w = w.split_once(':').unwrap_or((w, "")).0;
            match w.parse::<i64>()
            {
                Ok(x) => { numbers.push(x); },
                Err(_) => ()
            };
        }
        blueprints.push(Blueprint {
            ore_robot: numbers[1],
            clay_robot: numbers[2],
            obsidian_robot: (numbers[3], numbers[4]),
            geode_robot: (numbers[5], numbers[6]),
            max_ore_needed: std::cmp::max(std::cmp::max(numbers[5], numbers[3]), numbers[2]) });
    }
    return blueprints;
}

fn part_a(content: &'static str) -> i64
{
    let blueprints = parse_blueprints(content);
    let mut qualities = 0;
    for i in 0..blueprints.len()
    {
        let mut curr_best = vec![0; 24];
        let amount = solve_recursive(&blueprints[i], State::new(), 24, &mut curr_best);
        qualities += amount * (i + 1) as i64;
    }

    return qualities;
}

#[test]
fn part_b_test()
{
    let value = part_b(&_TEST_DATA);
    assert_eq!(value, 56 * 62);
}

fn part_b(content: &'static str) -> i64
{
    let blueprints = parse_blueprints(content);
    let mut geodes = 1;
    for i in 0..std::cmp::min(3, blueprints.len())
    {
        let mut curr_best = vec![0; 32];
        let amount = solve_recursive(&blueprints[i], State::new(), 32, &mut curr_best);
        geodes *= amount;
    }

    return geodes;
}

