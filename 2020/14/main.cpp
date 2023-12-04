
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>




std::string parseName(const char **ptr)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p && *p != ' ')
	{
		name += *p++;
	}
	if(*p == '\0')
		return name;
	name += *p++;
	while(*p != ' ')
	{
		name += *p++;
	}
	if(*p == ' ')
		p++;
	
	return name;
}

int64_t parseNumber(const char **ptr)
{
	const char *&p = ptr[0];
	int64_t number = 0;
	bool neg = false;
	if(*p == '-')
	{
		neg = true;
		p++;
	}

	while(isdigit(*p))
		number = number * 10 + (*p++) - '0';
	if(*p == ' ')
		p++;
	return neg ? -number : number;
}

void findSpace(const char **ptr)
{
	const char *&p = ptr[0];
	while(!(*p == '\0' || *p == ' '))
		p++;
	if(*p == ' ')
		p++;
}

int readValuesPartA(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}

	int64_t andMask = 0;
	int64_t orMask = 0;

	std::unordered_map<int64_t, int64_t> memory;

	std::string s;
	while(getline(f, s))
	{
		const char *ptr = &s[0];
		if(strncmp(ptr, "mask = ", 7) == 0)
		{
			ptr += 7;
			andMask = 0;
			orMask = 0;
			while(*ptr != '\0')
			{
				andMask <<= 1;
				orMask <<= 1;
				if(*ptr == '0')
				{
				}
				else if(*ptr == '1')
				{
					andMask |= 1;
					orMask |= 1;
				}
				else
				{
					andMask |= 1;
				}


				++ptr;
			}
			ptr = &s[7];
		}
		else if(strncmp(ptr, "mem[", 4) == 0)
		{
			ptr += 4;
			int64_t memAddress = parseNumber(&ptr);
			while(!isdigit(*ptr)) ++ptr;
			int64_t value = parseNumber(&ptr);
			value |= orMask;
			value &= andMask;
			memory[memAddress] = value;
		}
		else if(*ptr != '\0')
		{
			assert(0);
		}
		
	}
	f.close();

	int64_t sumOfMemory = 0;
	for(const auto p : memory)
	{
		sumOfMemory += p.second;
	}
	printf("Sum of all values, part a: %" PRIi64 "\n", sumOfMemory);

	return 0;
}

// Set value recursively, split into 2 on every fluctmask 1
void setRecursiveMemory(std::unordered_map<int64_t, int64_t> &memory, 
	const int64_t value, const int64_t fluctMask, int64_t memAddress, int64_t currentHighestBit)
{
	if(currentHighestBit >= 40)
	{
		memory[memAddress] = value;
		return;
	}
	if((fluctMask >> currentHighestBit) & 1)
	{
		setRecursiveMemory(memory, value, fluctMask,
			(memAddress | (int64_t(1) << (currentHighestBit))), 
			currentHighestBit + 1);
	}
	setRecursiveMemory(memory, value, fluctMask, memAddress, currentHighestBit + 1);
}

int readValuesPartB(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}

	int64_t orMask = 0;
	int64_t fluctMask = 0;

	std::unordered_map<int64_t, int64_t> memory;

	std::string s;
	while(getline(f, s))
	{
		const char *ptr = &s[0];
		if(strncmp(ptr, "mask = ", 7) == 0)
		{
			ptr += 7;
			fluctMask = 0;
			orMask = 0;
			while(*ptr != '\0')
			{
				fluctMask <<= 1;
				orMask <<= 1;
				if(*ptr == '0')
				{
				}
				else if(*ptr == '1')
				{
					orMask |= 1;
				}
				else
				{
					fluctMask |= 1;
				}


				++ptr;
			}
			ptr = &s[7];
		}
		else if(strncmp(ptr, "mem[", 4) == 0)
		{
			ptr += 4;
			int64_t memAddress = parseNumber(&ptr);
			while(!isdigit(*ptr)) ++ptr;
			int64_t value = parseNumber(&ptr);
			memAddress |= orMask;
			memAddress &= ~fluctMask;
			setRecursiveMemory(memory, value, fluctMask, memAddress, 0);
		}
		else if(*ptr != '\0')
		{
			assert(0);
		}
		
	}
	f.close();

	int64_t sumOfMemory = 0;
	for(const auto p : memory)
	{
		sumOfMemory += p.second;
	}
	printf("Sum of all values, part b: %" PRIi64 "\n", sumOfMemory);

	return 0;
}


int main(int argc, char** argv)
{
	if(readValuesPartA("data.txt") == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	if(readValuesPartB("data.txt") == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	return 0;
}