use std::io::BufRead;

// The output is wrapped in a Result to allow matching on errors
// Returns an Iterator to the Reader of the lines of the file.
pub fn read_lines<P>(filename: P) ->
    std::io::Result<std::io::Lines<std::io::BufReader<std::fs::File>>>
    where P: AsRef<std::path::Path>,
{
    let file = std::fs::File::open(filename)?;
    Ok(std::io::BufReader::new(file).lines())
}

pub fn read_line_by_line<T>(filename: &str, mut func: T) -> bool where T: FnMut(&str) -> bool
{
    if let Ok(lines) = read_lines(filename)
    {
        // Consumes the iterator, returns an (Optional) String
        for line in lines
        {
            if let Ok(line_string) = line
            {
                if func(&line_string) == false
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}