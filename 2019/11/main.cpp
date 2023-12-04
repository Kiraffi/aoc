#include <algorithm>
#include <cassert>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

#define SHOW_COMMAND_OUTPUT 0

using INT_MEMORY = int64_t;

static bool readValues(const char *filename, std::vector<INT_MEMORY> &outValues);
struct MachineState;

using InstructionFunction = bool(*)(MachineState &state);





struct MachineState
{

	std::vector<INT_MEMORY> memory;
	INT_MEMORY *functionParameterPointers[4] = {};
	int instructionPointer = 0;
	std::vector<INT_MEMORY> inputs;
	std::vector<INT_MEMORY> outputs;
	int relativeBase = 0;
	bool halted = false;
};


struct Instruction
{
	int opcode = 0;
	int parameters = 0;
	InstructionFunction func;
};


static bool opcodeAdd(MachineState &state)
{
	INT_MEMORY left = *state.functionParameterPointers[0];
	INT_MEMORY right = *state.functionParameterPointers[1];

	*state.functionParameterPointers[2] = left + right;
	#if SHOW_COMMAND_OUTPUT
		printf("Add: %" PRId64 " + %" PRId64 " = %" PRId64 "\n", left, right, 
			*state.functionParameterPointers[2]);
	#endif
	return true;
}

static bool opcodeMul(MachineState &state)
{
	INT_MEMORY left = *state.functionParameterPointers[0];
	INT_MEMORY right = *state.functionParameterPointers[1];
	*state.functionParameterPointers[2] = left * right;
	#if SHOW_COMMAND_OUTPUT
		printf("Mul: %" PRId64 " * %" PRId64 " = %" PRId64 "\n", left, right, 
			*state.functionParameterPointers[2]);
	#endif
	return true;
}

static bool opcodeInput(MachineState &state)
{
	#if SHOW_COMMAND_OUTPUT
		printf("read input: %" PRId64 "\n", state.inputs[0]);
	#endif
	*state.functionParameterPointers[0] = state.inputs[0];
	state.inputs.erase(state.inputs.begin());
	return true;
}

static bool opcodeOutput(MachineState &state)
{
	#if SHOW_COMMAND_OUTPUT
		printf("output: %" PRId64 "\n", *state.functionParameterPointers[0]);
	#endif
	state.outputs.push_back(*state.functionParameterPointers[0]);
	return true;
}

static bool opcodeJumpIfTrue(MachineState &state)
{
	INT_MEMORY check = *state.functionParameterPointers[0];
	INT_MEMORY address = *state.functionParameterPointers[1];

	#if SHOW_COMMAND_OUTPUT
		printf("if %" PRId64 " is not 0, jump into %" PRId64 "\n", check, address);
	#endif
	if(check != 0)
	{
		state.instructionPointer = address;
	}
	return true;
}

static bool opcodeJumpIfFalse(MachineState &state)
{
	INT_MEMORY check = *state.functionParameterPointers[0];
	INT_MEMORY address = *state.functionParameterPointers[1];
	
	#if SHOW_COMMAND_OUTPUT
		printf("if %" PRId64 " is 0, jump into %" PRId64 "\n", check, address);
	#endif
	if(check == 0)
	{
		state.instructionPointer = address;
	}
	return true;
}

static bool opcodeLessThan(MachineState &state)
{
	INT_MEMORY left = *state.functionParameterPointers[0];
	INT_MEMORY right = *state.functionParameterPointers[1];
	int storeValue = left < right ? 1 : 0;

	#if SHOW_COMMAND_OUTPUT
		printf("check less than: %" PRId64 " < %" PRId64 " = %i\n", left, right, storeValue);
	#endif
	*state.functionParameterPointers[2] = storeValue;
	return true;
}

static bool opcodeEquals(MachineState &state)
{
	INT_MEMORY left = *state.functionParameterPointers[0];
	INT_MEMORY right = *state.functionParameterPointers[1];
	int storeValue = left == right ? 1 : 0;  
	#if SHOW_COMMAND_OUTPUT
		printf("check equal: %" PRId64 " == %" PRId64 " = %i\n", left, right, storeValue);
	#endif
	*state.functionParameterPointers[2] = storeValue;

	return true;
}

static bool opcondeAddRelativeBase(MachineState &state)
{
	INT_MEMORY adjust = *state.functionParameterPointers[0];
	state.relativeBase += adjust;
	#if SHOW_COMMAND_OUTPUT
		printf("Adjusting relative base by: %" PRId64 ", base: %" PRId64 "\n", adjust, state.relativeBase);
	#endif
	return true;
}


static bool opcodeHalt(MachineState &state)
{
	printf("Halted\n");
	state.halted = true;
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
	Instruction{9, 1, opcondeAddRelativeBase},
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

static void program(MachineState &state)
{
	if(state.halted)
		return;

	int &pointer = state.instructionPointer;
	while(pointer < state.memory.size() && pointer >= 0)
	{
		INT_MEMORY fullOpCode = state.memory[pointer];
		INT_MEMORY opcode = fullOpCode % 100;
		const Instruction &instruction = instructions[opcode];
		#if SHOW_COMMAND_OUTPUT
			printf("Address: %" PRId64 " - ", pointer);
		#endif
		if(opcode == 3 && state.inputs.size() == 0)
		{
			return;
		}
		if(pointer + instruction.parameters + 1 > state.memory.size())
		{
			INT_MEMORY newSize = (pointer + instruction.parameters + 1) * 1.25f;
			printf(" Adjusting memory size to: %" PRId64 " ", newSize);
			#if !SHOW_COMMAND_OUTPUT
				printf("\n");
			#endif
			state.memory.resize(newSize, 0);
		}
		INT_MEMORY immediatePart = fullOpCode / 100;
		for(int param = 0; param < instruction.parameters; ++param)
		{
			INT_MEMORY memoryAccessType = (immediatePart % 10);
			INT_MEMORY address = state.memory[pointer + param + 1];
			switch(memoryAccessType)
			{
				// using fallthrough.
				case 2: // relative
				{
					#if SHOW_COMMAND_OUTPUT
						printf(" relative ");
					#endif
					address += state.relativeBase;
				}
				case 0: // position
				{
					if(address >= state.memory.size())
					{
						INT_MEMORY newSize = address * 1.25f;
						printf(" Adjusting memory size to: %" PRId64 " ", newSize);
						#if !SHOW_COMMAND_OUTPUT
							printf("\n");
						#endif

						state.memory.resize(newSize, 0);
					}
					else if(address < 0)
					{
						printf("Pointing negative address!\n");
						assert(false && "Negative address!");
						return;
					}
					
				}
				break;
				
				case 1: // immediate
				{
					address = pointer + param + 1;
				}
				break;

			}
			#if SHOW_COMMAND_OUTPUT
				printf(" param %i address: %i ", param, address);
			#endif
			state.functionParameterPointers[param] = &state.memory[address];;
			immediatePart /= 10;
		}

		pointer += instruction.parameters + 1;
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

void robot(const std::vector<INT_MEMORY> &memory, int startCode)
{
	MachineState state{memory, {}, 0, {}, {}, 0, false};
	
	std::vector<std::string> map;
	map.resize(1, std::string(1, '.'));

	int posX = 0;
	int posY = 0;
	int dir = 0;

	char charDir = 'A';

	state.inputs.push_back(startCode);
	while(!state.halted)
	{
		program(state);
		printf("\n");
		for(int i = 0; i < state.outputs.size(); ++i)
			printf("%" PRId64 "\n", state.outputs[i]);
		printf("\n");
		for(int j = 0; j < state.outputs.size() / 2; ++j)
		{
			printf("\n");
			int output1 = state.outputs[j * 2 + 0]; 
			if(output1 == 1)
				map[posY][posX] = '#';
			else if (output1 == 0)
				map[posY][posX] = ':';
			
			int output2 = state.outputs[j * 2 + 1]; 
			if(output2 == 0)
				--dir;
			else if(output2 == 1)
				++dir;
			
			while(dir < 0)
				dir += 4;
			while(dir >= 4)
				dir -= 4;

			switch(dir)
			{
				case 0:
					--posY;
					charDir = 'A';
					break;
				case 1:
					++posX;
					charDir = '>';
					break;
				case 2:
					++posY;
					charDir = 'v';
					break;
				case 3:
					charDir = '<';
					--posX;
					break;
				default:
					assert(false && "unknown direction");
					break;
			}
			if(posX < 0)
			{
				for(auto &s : map)
					s.insert(s.begin(), '.');
				++posX;
			}
			if(posX >= map[0].size())
			{
				for(auto &s : map)
					s += '.';
			}
			if(posY < 0)
			{
				map.insert(map.begin(), std::string(map[0].size(), '.'));
				++posY;
			}
			if(posY >= map.size())
			{
				map.push_back(std::string(map[0].size(), '.'));
			}
			assert(posX >= 0 && posY >= 0 && posX < map[0].size() && posY < map.size());
			char savedChar = map[posY][posX];
			
			map[posY][posX] = charDir;
			for(int s = 0; s < map.size(); ++s)
				printf("%s\n", map[s].c_str());
			printf("\n");
			map[posY][posX] = savedChar;
			
			if(map[posY][posX] == '.' || map[posY][posX] == ':')
				state.inputs.push_back(0);
			else
				state.inputs.push_back(1);
			
		}
		state.outputs.clear();
	}
	
	int painted = 0;
	for(const auto &s : map)
	{
		for(const auto c : s)
		{
			if(c != '.')
				++painted;
			
		}
		printf("%s\n", s.c_str());
	}
	printf("Painted: %i\n", painted);
	printf("Output amount: %i\n", state.outputs.size());	
}


int main(int argc, char** argv)
{
	std::vector<INT_MEMORY> memory;
	if(!readValues("data.txt", memory))
	{
		return 0;
	}

	if(memory.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}

	robot(memory, 0);
	robot(memory, 1);
	return 0;
	
}







static bool readValues(const char *filename, std::vector<INT_MEMORY> &outValues)
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
		INT_MEMORY v = 0;
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

