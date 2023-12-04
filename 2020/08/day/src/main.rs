use std::fs::File;
use std::io::{self, BufRead};

enum OpCode 
{
	OpNop,
	OpAdd,
	OpJmp
}
struct Command 
{
	op: OpCode,
	value: i32,
}

fn main() 
{
	let mut program: Vec<Command> = Vec::new();

	let file = File::open("src/data.txt").unwrap(); 
	for line in io::BufReader::new(file).lines()
	{
		let line = line.unwrap();
		let words: Vec<&str> = line.split_whitespace().collect();

		let v: i32 = words[1].parse().unwrap();
		match words[0]
		{
			"nop" => program.push(Command{op: OpCode::OpNop, value: v}),
			"acc" => program.push(Command{op: OpCode::OpAdd, value: v}),
			"jmp" => program.push(Command{op: OpCode::OpJmp, value: v}),
			_ => { panic!("unknown opcode"); }
		};
	}

	{
		let (_, accum) = exec_program(&program);
		println!("Part a, accumulated: {}", accum);
	}

	{
		for i in 0..program.len()
		{
			let mut change = true;
			match program[i].op
			{
				OpCode::OpNop => program[i].op = OpCode::OpJmp,
				OpCode::OpJmp => program[i].op = OpCode::OpNop,
				OpCode::OpAdd => change = false
			}
			if change
			{
				let (result, accum) = exec_program(&program);
				if result == 1
				{
					println!("Part b, accumulated: {}", accum);
					break;
				}
				match program[i].op
				{
					OpCode::OpNop => program[i].op = OpCode::OpJmp,
					OpCode::OpJmp => program[i].op = OpCode::OpNop,
					_ => {} 
				}
			}
		}
	}
}

fn exec_program(program: &Vec<Command>) -> (i32, i32)
{
	let mut program_ip: i32 = 0;
	let mut accum: i32 = 0;
	let mut visited: Vec<bool> = vec![false; program.len()];
	loop 
	{
		if program_ip >= program.len() as i32
		{
			return (1, accum);
		}

		if visited[program_ip as usize]
		{
			return (-1, accum);
		}
		
		visited[program_ip as usize] = true;

		let program_line = &program[program_ip as usize];
		let (new_acc, new_address ) = match program_line.op
		{
			OpCode::OpNop => (accum, program_ip + 1),  
			OpCode::OpAdd => (accum + program_line.value, program_ip + 1),
			OpCode::OpJmp => (accum, program_ip + program_line.value),
		};
		accum = new_acc;
		program_ip = new_address;
	}
}