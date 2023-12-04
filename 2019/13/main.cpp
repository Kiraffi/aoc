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
	return state.outputs.size() - state.outputPointer - 1;
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
	printf(" Adjusting memory size to: %" PRId64 " ", newSize);
#if !SHOW_COMMAND_OUTPUT
	printf("\n");
#endif

	state.memory.resize(newSize, 0);
}

static void program(MachineState &state)
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

static int firstRun(MachineState &state, std::vector<std::string> &outRoom)
{
	int blocks = 0;

	outRoom.push_back("0");
	int roomWidth = 1;
	int roomHeight = 1;

	program(state);
	while (getOutputAmount(state) > 2)
	{
		int posx = getOutput(state);
		int posy = getOutput(state);
		int type = getOutput(state);
		if (posx >= roomWidth)
		{
			for (std::string &s : outRoom)
				s += std::string(posx - s.length() + 1, ' ');
			roomWidth = posx + 1;
		}
		if (posy >= roomHeight)
		{
			for (int i = roomHeight; i <= posy; ++i)
				outRoom.push_back(std::string(roomWidth, ' '));
			roomHeight = posy + 1;
		}
		static constexpr char chars[] = {' ', '#', 'X', '=', 'O'};
		outRoom[posy][posx] = chars[type];
		if(type == 2)
			++blocks;
	}
	return blocks;
}

static void runGame(MachineState &state)
{
	std::vector<std::string> room;

	struct IVec2
	{
		int x = 0;
		int y = 0;
	};

	IVec2 ballPos;
	IVec2 newBallPos;
	IVec2 ballVel;

	IVec2 paddlePos;
	IVec2 newPaddlePos;



	int blocks = firstRun(state, room);
	state.inputs.push_back(0);

	// find ballpos
	for(int j = 0; j < room.size(); ++j)
	{
		for(int i = 0; i < room[j].size(); ++i)
		{
			if(room[j][i] == 'O')
			{
				newBallPos.x = ballPos.x = i;
				newBallPos.y = ballPos.y = j;
			}
		}
	}

	std::vector<int> velocityXMemoryAddresses;
	std::vector<int> velocityYMemoryAddresses;


	std::vector<int> scoreMemoryAddresses;
	// Scan memory for 0s to get the score memory
	for(int i = 0; i < state.memory.size(); ++i)
	{
		if(state.memory[i] == 0)
		{
			scoreMemoryAddresses.push_back(i);
		}
		if(state.memory[i] == 1)
			velocityXMemoryAddresses.push_back(i);
		if(state.memory[i] == 1)
			velocityYMemoryAddresses.push_back(i);

	}

	while (!state.halted)
	{
		program(state);
		while (getOutputAmount(state) > 2)
		{
			int posx = getOutput(state);
			int posy = getOutput(state);
			int type = getOutput(state);

			if (posx == -1 && posy == 0)
			{
				// Evil... the program doesn't actually output final score, but
				// you have to search it from program memory.

				// Find in memory where the score is being held by tracking down
				// all memory addresses and finding one that keeps getting updated
				for(int i = scoreMemoryAddresses.size(); i > 0; --i)
				{
					if(state.memory[scoreMemoryAddresses[i - 1]] != type)
					{
						scoreMemoryAddresses.erase(scoreMemoryAddresses.begin() + i - 1);
					}
				}

			}
			else
			{
				if (type == 3)
				{
					room[posy][posx] = '=';
					room[paddlePos.y][paddlePos.x] = ' ';
					paddlePos.x = posx;
					paddlePos.y = posy;
				}
				else if (type == 4)
				{
					room[posy][posx] = 'O';
					room[ballPos.y][ballPos.x] = ' ';
					newBallPos.x = posx;
					newBallPos.y = posy;
				}
			}
		}

		if(ballVel.x == 0)
			ballVel.x = newBallPos.x - ballPos.x;
		if(ballVel.y == 0)
			ballVel.y = newBallPos.y - ballPos.y;


		// Check that our prediction where the ball would be is correct.
		assert(ballPos.x + ballVel.x == newBallPos.x);
		assert(ballPos.y + ballVel.y == newBallPos.y);


		auto checkNextHit = [](int x, int y, std::vector<std::string> &room) 
		{
			bool result = true;
			if(y < 0 || x < 0 || y >= room.size() || x >= room[y].size())
				result = false;
			else
			{
				char c = room[y][x];
				result = c == 'X' || c == '#' || c == '=';
				if(c == 'X')
					room[y][x] = ' ';
			}
			return result;
		};

		bool dirChanged = true;
		// Check first x then y then corners, keep bouncing the ball as long as
		// there is something hitting.
		while(dirChanged)
		{
			int multis[] = {1,0, 0,1, 1,1};
			dirChanged = false;
			for(int i = 0; i < 3; ++i)
			{
				if(checkNextHit(newBallPos.x + ballVel.x * multis[i * 2 + 0], 
					newBallPos.y + ballVel.y * multis[i * 2 + 1], room))
				{
					ballVel.x = ballVel.x - ballVel.x * 2 * multis[i * 2 + 0];
					ballVel.y = ballVel.y - ballVel.y * 2 * multis[i * 2 + 1];
					dirChanged = true;
				}
			}
		}


		// scan memory for velocity value, another way to solve? or maybe the more correct?
		// no need to duplicate game logic.
		{
			if(ballVel.x != 0 && ballVel.y != 0)
			{
				for(int i = velocityXMemoryAddresses.size(); i > 0; --i)
				{
					if(state.memory[velocityXMemoryAddresses[i - 1]] != ballVel.x)
					{
						velocityXMemoryAddresses.erase(velocityXMemoryAddresses.begin() + i - 1);
					}
				}
				for(int i = velocityYMemoryAddresses.size(); i > 0; --i)
				{
					if(state.memory[velocityYMemoryAddresses[i - 1]] != ballVel.y)
					{
						velocityYMemoryAddresses.erase(velocityYMemoryAddresses.begin() + i - 1);
					}
				}
			}
		}


		int xDiff = paddlePos.x - newBallPos.x;
		xDiff -= ballVel.x;

		int movement = 0;
		if (xDiff > 0)
			movement = -1;
		else if (xDiff < 0)
			movement = 1;

		ballPos.x = newBallPos.x;
		ballPos.y = newBallPos.y;
		addInput(state, movement);

		room[ballPos.y][ballPos.x] = 'O';
	}
	assert(scoreMemoryAddresses.size() != 1);
	assert(scoreMemoryAddresses[0] < state.memory.size());
	printf("Final score: %i\n", state.memory[scoreMemoryAddresses[0]]);
	printf("Blocks: %i\n", blocks);
}

int main(int argc, char **argv)
{
	std::vector<INT_MEMORY> memory;
	if (!readValues("data.txt", memory))
	{
		return 0;
	}

	if (memory.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}
	
	{
		MachineState state{memory};
		state.memory[0] = 2;
		runGame(state);
	}

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
