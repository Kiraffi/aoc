
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
	static constexpr int PREAMBLE = 25;
	for(int i = PREAMBLE; i < numbers.size(); ++i)
	{
		int64_t searchValue = numbers[i];
		std::vector<int64_t> subNumbers;
		subNumbers.insert(subNumbers.begin(), 
			numbers.begin() + i - PREAMBLE, numbers.begin() + i);

		std::sort(subNumbers.begin(), subNumbers.end());
		int beginIndex = 0;
		int endIndex = PREAMBLE - 1;

		auto hasSum = [&]()
		{
			while(beginIndex < endIndex)
			{
				int64_t sum = subNumbers[beginIndex] + subNumbers[endIndex];
				if(sum < searchValue)
					++beginIndex;
				else if(sum > searchValue)
					--endIndex;
				else 
					return true;
			}
			return false;
		};
		if(!hasSum())
		{
			printf("Part: a: first number: %" PRIi64 "\n", searchValue);
			
			int64_t sum = numbers[0];
			int lowIndex = 0;
			int highIndex = 1;
			while(sum != searchValue)
			{
				if(sum > searchValue)
				{
					sum -= numbers[lowIndex];
					++lowIndex;
				}
				else
				{
					sum += numbers[highIndex];
					++highIndex;
				}
			}

			std::sort(numbers.begin() + lowIndex, numbers.begin() + highIndex);
			printf("Sum: %" PRIi64 "\n", numbers[lowIndex] + numbers[highIndex - 1]);

			break;
		}
	}

	return 0;
}