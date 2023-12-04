
const RUN_AMOUNT:u32 = 1;
const DAY_STR: &'static str = "08";

fn main()
{
    let _test_data = include_str!("../test_data.txt");
    let _data = include_str!("../../data/day08.txt");

    let now = std::time::Instant::now();
    /*    let mut top_visible = true;
    let mut bot_visible = true;

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

struct Tree
{
    height: u8,
    visible: bool
}
fn parse_trees(content: &str) -> Vec<Vec<Tree>>
{
    let mut trees: Vec<Vec<Tree>> = Vec::new();
    for line in content.lines()
    {
        trees.push(Vec::new());
        let v = trees.last_mut().unwrap();
        for &c in line.as_bytes()
        {
            v.push(Tree{height: c - '0' as u8, visible: false});
        }
    };
    return trees;
}

fn get_tree_height_for_pos(posx: usize, posy: usize, trees: &Vec<Vec<Tree>>) -> u8
{
    return trees[posy][posx].height;
}

fn check_visibilities_outside(trees: &mut Vec<Vec<Tree>>)
{
    let tree_cols = trees[0].len();
    let tree_rows = trees.len();

    for j in 0..tree_rows
    {
        let mut left_heightest = 0u8;
        let mut right_heightest = 0u8;
        for i in 0..tree_cols
        {
            let left = i;
            let right = tree_cols - 1 - i;

            let left_height = get_tree_height_for_pos(left, j, trees);
            let right_height = get_tree_height_for_pos(right, j, trees);

            if left_height > left_heightest || i == 0
            {
                trees[j][left].visible |= true;
                left_heightest = left_height;
            }
            if right_height > right_heightest || i == 0
            {
                trees[j][right].visible |= true;
                right_heightest = right_height;
            }
        }
    }

    for i in 0..tree_cols
    {
        let mut top_heightest = 0u8;
        let mut bot_heightest = 0u8;
        for j in 0..tree_rows
        {
            let top = j;
            let bot = tree_rows - 1 - j;
            let top_height = get_tree_height_for_pos(i, top, trees);
            let bot_height = get_tree_height_for_pos(i, bot, trees);
            if top_height > top_heightest || j == 0
            {
                trees[top][i].visible |= true;
                top_heightest = top_height;
            }
            if bot_height > bot_heightest || j == 0
            {
                trees[bot][i].visible |= true;
                bot_heightest = bot_height;
            }
        }
    }
}

fn part_a(print_outcome: bool, content: &'static str)
{
    let mut trees = parse_trees(content);
    check_visibilities_outside(&mut trees);

    let mut visible_trees = 0;
    for t1 in trees
    {
        for t2 in t1
        {
            if t2.visible
            {
                visible_trees += 1;
            }
        }
    }

    if print_outcome
    {
        println!("Day {}-1: Visible trees: {}", DAY_STR, visible_trees);
    }
}

fn calculate_scenic_score_for_pos(posx: usize, posy: usize, trees: &Vec<Vec<Tree>>) -> usize
{
    let tree_cols = trees[0].len();
    let tree_rows = trees.len();

    let height = get_tree_height_for_pos(posx, posy, trees);

    let mut scenic_score = 1;
    let mut dir_score = 0;
    if posx > 0
    {
        for i in 0..posx
        {
            dir_score += 1;
            let left = posx - 1 - i;
            let cur_height = get_tree_height_for_pos(left, posy, trees);
            if cur_height >= height
            {
                break;
            }
        }
        scenic_score = dir_score;
        dir_score = 0;
    }
    if posx + 1 < tree_cols
    {
        for i in posx + 1..tree_cols
        {
            dir_score += 1;
            let cur_height = get_tree_height_for_pos(i, posy, trees);
            if cur_height >= height
            {
                break;
            }
        }
        scenic_score *= dir_score;
        dir_score = 0;
    }
    if posy > 0
    {
        for j in 0..posy
        {
            dir_score += 1;
            let up = posy - 1 - j;
            let cur_height = get_tree_height_for_pos(posx, up, trees);
            if cur_height >= height
            {
                break;
            }
        }
        scenic_score *= dir_score;
        dir_score = 0;
    }
    if posy + 1 < tree_rows
    {
        for j in posy + 1..tree_rows
        {
            dir_score += 1;
            let cur_height = get_tree_height_for_pos(posx, j, trees);
            if cur_height >= height
            {
                break;
            }
        }
        scenic_score *= dir_score;
    }
    return scenic_score;
}

fn part_b(print_outcome: bool, content: &'static str)
{
    let trees = parse_trees(content);
    let mut scenic_score = 0;

    let tree_cols = trees[0].len();
    let tree_rows = trees.len();

    for j in 1..tree_rows - 1
    {
        for i in 1..tree_cols -1
        {
            let curr_scenic_score = calculate_scenic_score_for_pos(i, j, &trees);
            scenic_score = std::cmp::max(scenic_score, curr_scenic_score);
        }
    }
    if print_outcome
    {
        println!("Day {}-2: Highest scenic score: {}", DAY_STR, scenic_score);
    }
}
