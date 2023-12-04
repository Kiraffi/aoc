
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

static bool readValues(const char *filename, std::vector<int> &outValues);
struct MachineState;

using InstructionFunction = bool(*)(MachineState &state);

struct MachineState
{
	std::vector<int> memory;
	int *functionParameterPointers[4];
	int instructionPointer;
	int input;
};


struct Instruction
{
	int opcode;
	int parameters;
	InstructionFunction func;
};


static bool opcodeAdd(MachineState &state)
{
	*state.functionParameterPointers[2] = *state.functionParameterPointers[0] + *state.functionParameterPointers[1];
	return true;
}

static bool opcodeMul(MachineState &state)
{
	*state.functionParameterPointers[2] = *state.functionParameterPointers[0] * *state.functionParameterPointers[1];
	return true;
}

static bool opcodeInput(MachineState &state)
{
	printf("input: %i\n", *state.functionParameterPointers[0]);
	*state.functionParameterPointers[0] = state.input;
	return true;
}

static bool opcodeOutput(MachineState &state)
{
	printf("output: %i\n", *state.functionParameterPointers[0]);
	return true;
}

static bool opcodeJumpIfTrue(MachineState &state)
{
	if(*state.functionParameterPointers[0] != 0)
	{
		state.instructionPointer = *state.functionParameterPointers[1];
	}
	return true;
}

static bool opcodeJumpIfFalse(MachineState &state)
{
	if(*state.functionParameterPointers[0] == 0)
	{
		state.instructionPointer = *state.functionParameterPointers[1];
	}
	return true;
}

static bool opcodeLessThan(MachineState &state)
{
	int storeValue = *state.functionParameterPointers[0] < *state.functionParameterPointers[1] ? 1 : 0;  
	*state.functionParameterPointers[2] = storeValue;

	return true;
}

static bool opcodeEquals(MachineState &state)
{
	int storeValue = *state.functionParameterPointers[0] == *state.functionParameterPointers[1] ? 1 : 0;  
	*state.functionParameterPointers[2] = storeValue;

	return true;
}

static bool opcodeHalt(MachineState &state)
{
	printf("Halted\n");
	return false;
}

static bool opcodeNotImplemented(MachineState &state)
{
	printf("Failed, unimplemented opcode!\n");
	return false;
}



static constexpr Instruction instructions[] = 
{
	Instruction{0, 0, opcodeNotImplemented},
	Instruction{1, 3, opcodeAdd},
	Instruction{2, 3, opcodeMul},
	Instruction{3, 1, opcodeInput},
	Instruction{4, 1, opcodeOutput},
	Instruction{5, 2, opcodeJumpIfTrue},
	Instruction{6, 2, opcodeJumpIfFalse},
	Instruction{7, 3, opcodeLessThan},
	Instruction{8, 3, opcodeEquals},
	Instruction{9, 0, opcodeNotImplemented},
	Instruction{10, 0, opcodeNotImplemented},
	Instruction{11, 0, opcodeNotImplemented},
	Instruction{12, 0, opcodeNotImplemented},
	Instruction{13, 0, opcodeNotImplemented},
	Instruction{14, 0, opcodeNotImplemented},
	Instruction{15, 0, opcodeNotImplemented},
	Instruction{16, 0, opcodeNotImplemented},
	Instruction{17, 0, opcodeNotImplemented},
	Instruction{18, 0, opcodeNotImplemented},
	Instruction{19, 0, opcodeNotImplemented},
	Instruction{20, 0, opcodeNotImplemented},
	Instruction{21, 0, opcodeNotImplemented},
	Instruction{22, 0, opcodeNotImplemented},
	Instruction{23, 0, opcodeNotImplemented},
	Instruction{24, 0, opcodeNotImplemented},
	Instruction{25, 0, opcodeNotImplemented},
	Instruction{26, 0, opcodeNotImplemented},
	Instruction{27, 0, opcodeNotImplemented},
	Instruction{28, 0, opcodeNotImplemented},
	Instruction{29, 0, opcodeNotImplemented},
	Instruction{30, 0, opcodeNotImplemented},
	Instruction{31, 0, opcodeNotImplemented},
	Instruction{32, 0, opcodeNotImplemented},
	Instruction{33, 0, opcodeNotImplemented},
	Instruction{34, 0, opcodeNotImplemented},
	Instruction{35, 0, opcodeNotImplemented},
	Instruction{36, 0, opcodeNotImplemented},
	Instruction{37, 0, opcodeNotImplemented},
	Instruction{38, 0, opcodeNotImplemented},
	Instruction{39, 0, opcodeNotImplemented},
	Instruction{40, 0, opcodeNotImplemented},
	Instruction{41, 0, opcodeNotImplemented},
	Instruction{42, 0, opcodeNotImplemented},
	Instruction{43, 0, opcodeNotImplemented},
	Instruction{44, 0, opcodeNotImplemented},
	Instruction{45, 0, opcodeNotImplemented},
	Instruction{46, 0, opcodeNotImplemented},
	Instruction{47, 0, opcodeNotImplemented},
	Instruction{48, 0, opcodeNotImplemented},
	Instruction{49, 0, opcodeNotImplemented},
	Instruction{50, 0, opcodeNotImplemented},
	Instruction{51, 0, opcodeNotImplemented},
	Instruction{52, 0, opcodeNotImplemented},
	Instruction{53, 0, opcodeNotImplemented},
	Instruction{54, 0, opcodeNotImplemented},
	Instruction{55, 0, opcodeNotImplemented},
	Instruction{56, 0, opcodeNotImplemented},
	Instruction{57, 0, opcodeNotImplemented},
	Instruction{58, 0, opcodeNotImplemented},
	Instruction{59, 0, opcodeNotImplemented},
	Instruction{60, 0, opcodeNotImplemented},
	Instruction{61, 0, opcodeNotImplemented},
	Instruction{62, 0, opcodeNotImplemented},
	Instruction{63, 0, opcodeNotImplemented},
	Instruction{64, 0, opcodeNotImplemented},
	Instruction{65, 0, opcodeNotImplemented},
	Instruction{66, 0, opcodeNotImplemented},
	Instruction{67, 0, opcodeNotImplemented},
	Instruction{68, 0, opcodeNotImplemented},
	Instruction{69, 0, opcodeNotImplemented},
	Instruction{70, 0, opcodeNotImplemented},
	Instruction{71, 0, opcodeNotImplemented},
	Instruction{72, 0, opcodeNotImplemented},
	Instruction{73, 0, opcodeNotImplemented},
	Instruction{74, 0, opcodeNotImplemented},
	Instruction{75, 0, opcodeNotImplemented},
	Instruction{76, 0, opcodeNotImplemented},
	Instruction{77, 0, opcodeNotImplemented},
	Instruction{78, 0, opcodeNotImplemented},
	Instruction{79, 0, opcodeNotImplemented},
	Instruction{80, 0, opcodeNotImplemented},
	Instruction{81, 0, opcodeNotImplemented},
	Instruction{82, 0, opcodeNotImplemented},
	Instruction{83, 0, opcodeNotImplemented},
	Instruction{84, 0, opcodeNotImplemented},
	Instruction{85, 0, opcodeNotImplemented},
	Instruction{86, 0, opcodeNotImplemented},
	Instruction{87, 0, opcodeNotImplemented},
	Instruction{88, 0, opcodeNotImplemented},
	Instruction{89, 0, opcodeNotImplemented},
	Instruction{90, 0, opcodeNotImplemented},
	Instruction{91, 0, opcodeNotImplemented},
	Instruction{92, 0, opcodeNotImplemented},
	Instruction{93, 0, opcodeNotImplemented},
	Instruction{94, 0, opcodeNotImplemented},
	Instruction{95, 0, opcodeNotImplemented},
	Instruction{96, 0, opcodeNotImplemented},
	Instruction{97, 0, opcodeNotImplemented},
	Instruction{98, 0, opcodeNotImplemented},
	Instruction{99, 0, opcodeHalt},
};

// pass by copy
static void program(MachineState state)
{
	std::vector<bool> immediates;
	state.instructionPointer = 0;
	int &pointer = state.instructionPointer;
	while(pointer < state.memory.size())
	{
		int fullOpCode = state.memory[pointer];
		++pointer;
		int opcode = fullOpCode % 100;
		const Instruction &instruction = instructions[opcode];
		if(pointer + instruction.parameters > state.memory.size())
		{
			printf("Parameters are out of memory range.\n");
			return;
		}
		int immediatePart = fullOpCode / 100;
		for(int param = 0; param < instruction.parameters; ++param)
		{
			bool immediate = (immediatePart % 10) == 1;
			int *tmp = &state.memory[pointer + param];
			state.functionParameterPointers[param] = tmp;
			if(!immediate)
			{
				if(*tmp < 0 || *tmp >= state.memory.size())
				{
					printf("Tried to point out of memory!\n");
					return;
				}

			}
			state.functionParameterPointers[param] = immediate ? tmp : &state.memory[*tmp];
			immediatePart /= 10;
		}

		pointer += instruction.parameters;
		bool continueExecution = instruction.func(state);
		if(!continueExecution)
		{
			return;
		}

	}

	printf("Trying to read op codes out of array: pointer: %i vs size: %i\n",
		pointer, int(state.memory.size()));
	return;
}

int main(int argc, char** argv)
{
	std::vector<int> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}

	program(MachineState{ values, {}, 0, 5});

	return 0;
}







static bool readValues(const char *filename, std::vector<int> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	while(getline(f, s))
	{
		int v = 0;
		bool neg = false;
		for(int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if(c >= '0' && c <= '9')
			{
				v = v * 10 + (c - '0');
			}
			if(c == '-')
			{
				neg = true;
			}
			if(c == ',' || i == s.length() - 1)
			{
				if(neg)
					v = -v;
				outValues.emplace_back(v);
				v = 0;
				neg = false;
			}
			
		}
	}
	f.close();
	
	return true;
}

