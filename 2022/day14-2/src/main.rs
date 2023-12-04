use macroquad::prelude::*;

const WIDTH: u16 = 1000;
const HEIGHT: u16 = 1000;

fn set_pixel_color(x: i32, y: i32, tex: &mut Image, col: Color)
{
    let x = x as u32;
    let y = y as u32;
    tex.set_pixel(x, y, col);
}

fn get_pixel(x: i32, y: i32, tex: &Image) -> char
{
    let x = x as u32;
    let y = y as u32;

    if tex.get_pixel(x, y).r == 0.0
    {
        return ' ';
    }
    return '#';
}
fn get_pixel_4(x: i32, y: i32, tex: &Image) -> char
{
    return get_pixel(x * 2 - 250, y * 2, tex);
}

fn get_pixel_9(x: i32, y: i32, tex: &Image) -> char
{
    return get_pixel(x * 3- 250, y * 3, tex);
}


fn set_pixel_color_4(x: i32, y: i32, tex: &mut Image, col: Color)
{
    set_pixel_color(x * 2 + 0 - 250, y * 2 + 0, tex, col);
    set_pixel_color(x * 2 + 0 - 250, y * 2 + 1, tex, col);
    set_pixel_color(x * 2 + 1 - 250, y * 2 + 0, tex, col);
    set_pixel_color(x * 2 + 1 - 250, y * 2 + 1, tex, col);
}

fn set_pixel_color_9(x: i32, y: i32, tex: &mut Image, col: Color)
{
    set_pixel_color(x * 3 + 0 - 250, y * 3 + 0, tex, col);
    set_pixel_color(x * 3 + 0 - 250, y * 3 + 1, tex, col);
    set_pixel_color(x * 3 + 0 - 250, y * 3 + 2, tex, col);
    set_pixel_color(x * 3 + 1 - 250, y * 3 + 0, tex, col);
    set_pixel_color(x * 3 + 1 - 250, y * 3 + 1, tex, col);
    set_pixel_color(x * 3 + 1 - 250, y * 3 + 2, tex, col);
    set_pixel_color(x * 3 + 2 - 250, y * 3 + 0, tex, col);
    set_pixel_color(x * 3 + 2 - 250, y * 3 + 1, tex, col);
    set_pixel_color(x * 3 + 2 - 250, y * 3 + 2, tex, col);
}

fn set_pixel(x: i32, y: i32, tex: &mut Image, value: char)
{
    match value
    {
        '#' => set_pixel_color_9(x, y, tex, Color{r: 1.0, g: 0.0, b: 0.0, a: 1.0}),
        '+' => set_pixel_color_9(x, y, tex, Color{r: 0.5, g: 1.0, b: 0.5, a: 1.0}),
        ' ' => set_pixel_color_9(x, y, tex, Color{r: 0.0, g: 0.0, b: 0.0, a: 1.0}),
        _ => ()
    }
}

fn reset_board(image: &mut Image)
{
    for y in 0..image.height
    {
        for x in 0..image.width
        {
            let c = image.get_pixel(x as u32, y as u32);
            if c.r > 0.1 && c.r < 0.9
            {
                image.set_pixel(x as u32, y as u32, BLACK);
            }
        }
    }
}


#[macroquad::main("Prelude SandStorm")]
async fn main()
{


    let data = include_str!("../../data/day14.txt");
    let mut tex: Image = Image::gen_image_color(WIDTH, HEIGHT, BLACK);

    let d = data.lines().for_each(|s|
        {
            s.split(" -> ").fold((0, 0), |prev, value|
            {
                let values: Vec<i32> = value.split(',')
                    .map(|x| {x.parse::<i32>().unwrap()})
                    .collect();
                if prev != (0, 0)
                {
                    let (mut x, mut y) = prev;
                    while x != values[0] && y != values[0]
                    {
                        set_pixel(x, y, &mut tex, '#');
                        let x_diff = values[0] - x;
                        let y_diff = values[1] - y;
                        x += if x_diff != 0 { x_diff / i32::abs(x_diff) } else {0};
                        y += if y_diff != 0 { y_diff / i32::abs(y_diff) } else {0};
                    }
                }

                return (values[0], values[1]);
            });
        });

//    .map(|p| {p.split_once})
    let mut last_update = 0f64;
    let mut sand_pos = (500, 0);
    let texture = Texture2D::from_image(&tex);
    loop {
        texture.update(&tex);
        if get_time() - last_update > 0.125f64 * 0.25
        {

            last_update = get_time();
            set_pixel(sand_pos.0, sand_pos.1, &mut tex, ' ');
            if sand_pos.1 >= 250
            {
                reset_board(&mut tex);
                sand_pos = (500, 0);
            }
            let dirs: Vec<(i32, i32)> = vec![(0, 1), (-1, 1), (1, 1)];
            let mut found = false;
            for d in dirs
            {
                if get_pixel_9(sand_pos.0 + d.0, sand_pos.1 + d.1, &tex) == ' '
                {
                    sand_pos.0 += d.0;
                    sand_pos.1 += d.1;
                    found = true;
                    break;
                }
            }
            if !found
            {
                set_pixel(sand_pos.0, sand_pos.1, &mut tex, '+');
                sand_pos = (500, 0);
            }
            set_pixel(sand_pos.0, sand_pos.1, &mut tex, '+');
        }
        clear_background(BLACK);
        draw_texture(texture, 0f32, 0f32, WHITE);
        next_frame().await
    }
}
