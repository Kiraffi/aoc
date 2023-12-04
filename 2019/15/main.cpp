#include <algorithm>
#include <cassert>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

#include <unordered_map>

#define SHOW_COMMAND_OUTPUT 0

using INT_MEMORY = int64_t;

static bool readValues(const char *filename, std::vector<INT_MEMORY> &outValues);
struct MachineState;

using InstructionFunction = bool (*)(MachineState &state);

struct MachineState
{

	std::vector<INT_MEMORY> memory;
	std::vector<INT_MEMORY> inputs;
	std::vector<INT_MEMORY> outputs;
	INT_MEMORY functionParameterAddresses[4] = {};
	INT_MEMORY instructionPointer = 0;
	INT_MEMORY relativeBase = 0;
	INT_MEMORY outputPointer = 0;
	INT_MEMORY inputPointer = 0;
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
	INT_MEMORY left = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY right = state.memory[state.functionParameterAddresses[1]];

	state.memory[state.functionParameterAddresses[2]] = left + right;
	#if SHOW_COMMAND_OUTPUT
		printf("Add: %" PRId64 " + %" PRId64 " = %" PRId64 "\n", left, right,
			   state.memory[state.functionParameterAddresses[2]]);
	#endif
	return true;
}

static bool opcodeMul(MachineState &state)
{
	INT_MEMORY left = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY right = state.memory[state.functionParameterAddresses[1]];
	state.memory[state.functionParameterAddresses[2]] = left * right;
	#if SHOW_COMMAND_OUTPUT
		printf("Mul: %" PRId64 " * %" PRId64 " = %" PRId64 "\n", left, right,
		   state.memory[state.functionParameterAddresses[2]]);
	#endif
	return true;
}

static bool opcodeInput(MachineState &state)
{
	assert(state.inputPointer < state.inputs.size());
	INT_MEMORY input = state.inputs[state.inputPointer];
	#if SHOW_COMMAND_OUTPUT
		printf("read input: %" PRId64 "\n", state.inputs[state.inputPointer]);
	#endif
	state.memory[state.functionParameterAddresses[0]] = input;
	state.inputPointer++;
	return true;
}

static bool opcodeOutput(MachineState &state)
{
	#if SHOW_COMMAND_OUTPUT
		printf("output: %" PRId64 "\n", state.memory[state.functionParameterAddresses[0]]);
	#endif
	state.outputs.push_back(state.memory[state.functionParameterAddresses[0]]);
	return true;
}

static bool opcodeJumpIfTrue(MachineState &state)
{
	INT_MEMORY check = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY address = state.memory[state.functionParameterAddresses[1]];

	#if SHOW_COMMAND_OUTPUT
		printf("if %" PRId64 " is not 0, jump into %" PRId64 "\n", check, address);
	#endif
	if (check != 0)
	{
		state.instructionPointer = address;
	}
	return true;
}

static bool opcodeJumpIfFalse(MachineState &state)
{
	INT_MEMORY check = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY address = state.memory[state.functionParameterAddresses[1]];

	#if SHOW_COMMAND_OUTPUT
		printf("if %" PRId64 " is 0, jump into %" PRId64 "\n", check, address);
	#endif
	if (check == 0)
	{
		state.instructionPointer = address;
	}
	return true;
}

static bool opcodeLessThan(MachineState &state)
{
	INT_MEMORY left = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY right = state.memory[state.functionParameterAddresses[1]];
	int storeValue = left < right ? 1 : 0;

	#if SHOW_COMMAND_OUTPUT
		printf("check less than: %" PRId64 " < %" PRId64 " = %i\n", left, right, storeValue);
	#endif
	state.memory[state.functionParameterAddresses[2]] = storeValue;
	return true;
}

static bool opcodeEquals(MachineState &state)
{
	INT_MEMORY left = state.memory[state.functionParameterAddresses[0]];
	INT_MEMORY right = state.memory[state.functionParameterAddresses[1]];
	int storeValue = left == right ? 1 : 0;
	#if SHOW_COMMAND_OUTPUT
		printf("check equal: %" PRId64 " == %" PRId64 " = %i\n", left, right, storeValue);
	#endif
	state.memory[state.functionParameterAddresses[2]] = storeValue;

	return true;
}

static bool opcondeAddRelativeBase(MachineState &state)
{
	INT_MEMORY adjust = state.memory[state.functionParameterAddresses[0]];
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

int getOutputAmount(MachineState &state)
{

	return state.outputs.size() - state.outputPointer;
}

static INT_MEMORY getOutput(MachineState &state)
{
	assert(state.outputPointer < state.outputs.size());
	INT_MEMORY output = state.outputs[state.outputPointer];
	++state.outputPointer;
	return output;
}

static void addInput(MachineState &state, INT_MEMORY input)
{
	state.inputs.push_back(input);
}

static void resizeMemory(MachineState &state, INT_MEMORY address)
{
	INT_MEMORY newSize = address * 1.25f;
	// Lets assume 1mb is enough in case some pointer goes out of control
	assert(address < (1 << 20));
	printf(" Adjusting memory size to: %" PRId64 " ", newSize);
	#if !SHOW_COMMAND_OUTPUT
		printf("\n");
	#endif

	state.memory.resize(newSize, 0);
}

static void executeProgram(MachineState &state)
{
	if (state.halted)
		return;

	INT_MEMORY &pointer = state.instructionPointer;
	while (pointer < state.memory.size() && pointer >= 0)
	{
		INT_MEMORY fullOpCode = state.memory[pointer];
		INT_MEMORY opcode = fullOpCode % 100;
		const Instruction &instruction = instructions[opcode];
		#if SHOW_COMMAND_OUTPUT
			printf("Address: %" PRId64 " - ", pointer);
		#endif
		if (opcode == 3 && state.inputPointer >= state.inputs.size())
		{
			return;
		}
		if (pointer + instruction.parameters + 1 > state.memory.size())
		{
			resizeMemory(state, pointer + instruction.parameters + 1);
		}
		INT_MEMORY immediatePart = fullOpCode / 100;
		for (int param = 0; param < instruction.parameters; ++param)
		{
			INT_MEMORY memoryAccessType = (immediatePart % 10);
			INT_MEMORY address = pointer + param + 1;

			// If the accesstype is immediate do not do anything to address
			if (memoryAccessType != 1)
			{
				address = state.memory[address];
				// For relative we should offset the address to get positional
				if (memoryAccessType == 2)
				{
					#if SHOW_COMMAND_OUTPUT
						printf(" relative ");
					#endif
					address += state.relativeBase;
				}

				// Now the address should be in positional

				// Resiaze memory if trying to access values outside the memory range
				if (address >= state.memory.size())
				{
					resizeMemory(state, address);
				}
				else if (address < 0)
				{
					printf("Pointing negative address!\n");
					assert(false && "Negative address!");
					return;
				}
			}
			#if SHOW_COMMAND_OUTPUT
				printf(" param %i address: %" PRId64 " ", param, address);
			#endif
			state.functionParameterAddresses[param] = address;
			immediatePart /= 10;
		}

		pointer += instruction.parameters + 1;
		bool continueExecution = instruction.func(state);
		if (!continueExecution)
		{
			return;
		}
	}

	printf("Trying to read op codes out of array: pointer: %" PRId64 " vs size: %zu\n",
		   pointer, state.memory.size());
	return;
}


enum Block
{
	ROBOT = -3,
	EMPTY_BLOCK = -2,
	WALL = -1
};

struct Map
{
	std::vector<std::vector<int>> map = {{0}};
	int offsetX = 0;
	int offsetY = 0;
	int width = 1;
	int height = 1;
	void doResize(int x, int y)
	{
		if(x + offsetX < 0)
		{
			int addAmount = -(x - offsetX);
			for(auto &s : map)
			{
				for(int i = 0; i < addAmount; ++i)
					s.insert(s.begin(), EMPTY_BLOCK);
			}
			offsetX += addAmount;
			width += addAmount;
		}
		if(x + offsetX >= width)
		{
			int addAmount = x + offsetX - width + 1;
			for(auto &s : map)
			{
				for(int i = 0; i < addAmount; ++i)
					s.push_back(EMPTY_BLOCK);
			}
			width += addAmount;
		}
		if(y + offsetY < 0)
		{
			int addAmount = -(y + offsetY);
			for(int i = 0; i < addAmount; ++i)
				map.insert(map.begin(), std::vector<int>(width, EMPTY_BLOCK));
			height += addAmount;
			offsetY += addAmount;
		}
		if(y + offsetY >= height)
		{
			int addAmount = y + offsetY - height + 1;
			for(int i = 0; i < addAmount; ++i)
				map.push_back(std::vector<int>(width, EMPTY_BLOCK));
			height += addAmount;

		}
	}
	
	int getMapTile(int x, int y)
	{
		doResize(x, y);
		return map[y + offsetY][x + offsetX];
	}
	void setMapTile(int x, int y, int c)
	{
		doResize(x, y);
		map[y + offsetY][x + offsetX] = c;
	}
	void print()
	{
		for(const auto &s : map)
		{
			for(const auto &t : s)
			{
				switch(t)
				{
					case EMPTY_BLOCK:
						printf(" ");
						break;
					case WALL:
						printf("#");
						break;
					case ROBOT:
						printf("D");
						break;
					default:
						printf("%i", t % 10);
						break;
				}
			}
			printf("\n");
		}
		printf("\n");
	}
};

MachineState oxygenState;

int recursiveFind(MachineState &state, int posX, int posY, Map &map, int distance, bool partA)
{
	int shortestDistance = (1 << 30);
	executeProgram(state);
	int outputs = getOutputAmount(state);
	while(outputs > 0)
	{
		INT_MEMORY output = getOutput(state);
		outputs = getOutputAmount(state); 
		if(output == 0)
		{
			// do nothing
			map.setMapTile(posX, posY, WALL);
			return WALL;
		}
		if(output == 1)
		{
			map.setMapTile(posX, posY, distance);
		}
		if(output == 2 && partA)
		{
			map.setMapTile(posX, posY, 'X');
			// copy our program at the state of being at oxygen.
			oxygenState = state;
			return distance;
		}
	}
	Map map2 = map;
	map2.setMapTile(posX, posY, -3);
	//map2.print();

	for(int i = 1; i <= 4; ++i)
	{
		int newX = posX;
		int newY = posY;
		if(i < 3)
			newY += (i - 1) * 2 - 1;
		else if(i < 5)
			newX += (i - 3) * 2 - 1;
		int c = map.getMapTile(newX, newY); 
		if(c == -2 || c > distance)
		{
			addInput(state, i);
			int res = recursiveFind(state, newX, newY, map, distance + 1, partA);

			// reverse action...
			if(res >= 0)
			{
				shortestDistance = res < shortestDistance ? res : shortestDistance;
				if(i < 3)
					addInput(state, 3 - i);
				else
					addInput(state, 7 - i);
			}
		}
	}

	return shortestDistance;
}

int main(int argc, char **argv)
{
	std::vector<INT_MEMORY> programMemory;
	if (!readValues("data.txt", programMemory))
	{
		return 0;
	}

	if (programMemory.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}
	
	MachineState state {programMemory};
	bool found = false;
	
	Map map;
	int shortestDistance = recursiveFind(state, 0, 0, map, 0, true);
	//map.print();

	printf("Shortest distance to oxygen: %i\n", shortestDistance);

	Map oxygenMap;
	recursiveFind(oxygenState, 0, 0, oxygenMap, 0, false);
	//oxygenMap.print();
	int highest = 0;
	for(const auto &r : oxygenMap.map)
	{
		for(int v : r)
		{
			highest = v > highest ? v : highest;
		}
	}
	printf("Oxygen takes to fill: %i minutes\n", highest);
	return 0;
}

static bool readValues(const char *filename, std::vector<INT_MEMORY> &outValues)
{
	std::ifstream f(filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	while (getline(f, s))
	{
		INT_MEMORY v = 0;
		bool neg = false;
		for (int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if (c >= '0' && c <= '9')
			{
				v = v * 10 + (c - '0');
			}
			if (c == '-')
			{
				neg = true;
			}
			if (c == ',' || i == s.length() - 1)
			{
				if (neg)
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
