
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

int readValues(const char *filename, std::vector<int64_t> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}
	std::string s;
	while(getline(f, s))
	{
		const char *ptr = s.data();
		while(*ptr != '\0')
		{
			outValues.push_back(parseNumber(&ptr));
			if(*ptr != '\0')
				++ptr;
		}
	}
	f.close();

	return 0;
}

int main(int argc, char** argv)
{
	std::vector<int64_t> numbers;
	if(readValues("data.txt", numbers) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	
	std::vector<int> allNumbers((1 << 25), -1);
	int64_t lastNumberIndex = -1;
	int64_t lastNumber = -1;
	int64_t index = 0;
	for(; index < numbers.size(); ++index)
	{
		lastNumber = numbers[index];
		lastNumberIndex = allNumbers[lastNumber];
		allNumbers[lastNumber] = index;
	}
	
	for(; index < 30000000; ++index)
	{
		if(index == 2020)
			printf("Part a: %" PRIi64 ", number: %" PRIi64 "\n", index, lastNumber);

		if(lastNumberIndex == -1)
		{
			lastNumber = 0;
			lastNumberIndex = allNumbers[lastNumber];
			allNumbers[lastNumber] = index;
		}
		else
		{
			lastNumber = (index - 1) - lastNumberIndex;
			assert(lastNumber < allNumbers.size());
			lastNumberIndex = allNumbers[lastNumber];
			allNumbers[lastNumber] = index;
		}
	}

	printf("Part b: %" PRIi64 ", number: %" PRIi64 "\n", index, lastNumber);

	return 0;
}