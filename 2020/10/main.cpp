
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>


std::string parseName(const char **ptr)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p != ' ')
	{
		name += *p++;
	}
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

int readValues(const char *filename, std::vector<int64_t> &outNumbers)
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
		outNumbers.push_back(parseNumber(&ptr));
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

	std::sort(numbers.begin(), numbers.end());
	int ones = 0;
	int threes = 1;
	int64_t prevNumber = 0;
	for(int i = 0; i < numbers.size(); ++i)
	{
		int64_t newNumber = numbers[i];
		if(newNumber == prevNumber + 1)
			++ones;
		else if(newNumber == prevNumber + 3)
			++threes;
		prevNumber = newNumber;
	}

	printf("ones: %i and threes: %i, multi: %i\n", ones, threes, ones * threes);


	numbers.insert(numbers.begin(), 0);
	std::vector<int64_t> rollingBuffer(4, 0);
	int64_t currentCount = 0;
	rollingBuffer[0] = 1;
	for(int i = 0; i < numbers.size(); ++i)
	{
		currentCount = rollingBuffer[i % 4];
		rollingBuffer[i % 4] = 0;
		int j = i + 1;
		while(j < numbers.size() && numbers[j] - numbers[i] <= 3)
		{
			rollingBuffer[j % 4] += currentCount;
			++j;
		}
	}

	printf("Combinations: % " PRIi64 "\n", currentCount);

	return 0;
}