
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

struct MinMax
{
	int minValue;
	int maxValue;
};

struct NameType
{
	std::string name;
	MinMax minMaxes[2];
};

std::string parseStringUntil(const char **ptr, char parseChar)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p && *p != parseChar)
	{
		name += *p++;
	}
	if(*p == '\0')
		return name;
	p++;

	return name;
}

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
	if(*p != '\0')
		p++;
	return neg ? -number : number;
}


std::vector<int> parseNumbers(const char **ptr)
{
	std::vector<int> result;
	const char *&p = ptr[0];
	bool valid = true;
	while(*p != '\0')
	{
		while(!isdigit(*p) && *p != '\0' && *p != '-') ++p;
		if(*p == '\0')
			break;
		result.push_back(parseNumber(&p));
	}
	return result;
}
std::vector<int> parseNumbers(const std::string &s)
{
	const char *ptr = s.data();
	return parseNumbers(&ptr);
}

void findSpace(const char **ptr)
{
	const char *&p = ptr[0];
	while(!(*p == '\0' || *p == ' '))
		p++;
	if(*p == ' ')
		p++;
}

int getFirstSetBit(int64_t v)
{
	int bitCount = 0;
	assert(v != 0);
	while((v & 1) == 0)
	{
		++bitCount;
		v = v >> 1;
	}
	return bitCount;
}

int64_t removeInvalids(const std::string &s, const std::vector<NameType> &nameTypes, 
	std::vector<int> &validNumbers)
{
	int64_t sum = 0;
	int numberIndex = 0;
	const char *ptr = s.data();
	while(*ptr != '\0')
	{
		int number = parseNumber(&ptr);

		int &p = validNumbers[numberIndex];
		++numberIndex;

		bool foundAny = false;
		int removes = 0;
		for(int i = 0; p >> i; ++i)
		{
			if(( (p >> i ) & 1) == 0)
				continue;
			const NameType &a = nameTypes[i];
			if((number >= a.minMaxes[0].minValue && number <= a.minMaxes[0].maxValue) || 
				(number >= a.minMaxes[1].minValue && number <= a.minMaxes[1].maxValue))
			{
				foundAny = true;
			}
			else
			{
				removes |= (1 << i);
			}
			
		}
		// didnt find any matching field
		if(!foundAny)
		{
			sum += number;
		}
		// if we found at least one matching remove the non matching ones.
		else
		{
			p &= ~removes;
			// Check that we dont remove last bit
			assert(p != 0);			
		}
	}
	return sum;
}

int readValues(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}

	std::vector<NameType> nameTypes;


	std::string s;
	while(getline(f, s))
	{
		const char *ptr = s.data();
		if(*ptr == '\0')
			break;
		nameTypes.emplace_back(NameType{});
		NameType &n = nameTypes[nameTypes.size() - 1];
		n.name = parseStringUntil(&ptr, ':');
		std::vector<int> numbers = parseNumbers(ptr);
		n.minMaxes[0] = {numbers[0], numbers[1]};
		n.minMaxes[1] = {numbers[2], numbers[3]};
	}
	getline(f, s);
	getline(f, s);
	std::vector<int> validNumbers(nameTypes.size(), (1 << nameTypes.size()) - 1);
	std::vector<int> myTicketNumbers = parseNumbers(s);
	int64_t sum = removeInvalids(s, nameTypes, validNumbers);
	getline(f, s);
	getline(f, s);

	

	while(getline(f, s))
	{
		sum += removeInvalids(s, nameTypes, validNumbers); 
	}

	f.close();


	printf("Part a: Invalid sum: %" PRIi64 "\n", sum);

	// just keep removing indices that have multiple hits, until we have 1 hit for
	// everything
	bool hasMultipleBitsSet = true;
	while(hasMultipleBitsSet)
	{
		hasMultipleBitsSet = false;
		for(int i = 0; i < validNumbers.size(); ++i)
		{
			int deleteValue = getFirstSetBit(validNumbers[i]);
			// Check that only one bit is set.
			if((validNumbers[i] - (1 << deleteValue)) != 0)
			{
				hasMultipleBitsSet = true;
				continue;
			}
			for(int j = 0; j < validNumbers.size(); ++j)
			{
				if(i == j)
					continue;
				validNumbers[j] &= ~(1 << deleteValue);
			}
		}
	}

	int64_t multi = 1;

	for(int i = 0; i < validNumbers.size(); ++i)
	{
		int j = getFirstSetBit(validNumbers[i]);
		if(nameTypes[j].name.compare(0, 9, "departure") == 0)
			multi *= myTicketNumbers[i];
	}
	printf("Part b: Departure multiples: %" PRIi64 "\n", multi);
	return 0;
}

int main(int argc, char** argv)
{
	if(readValues("data.txt") == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	

	return 0;
}