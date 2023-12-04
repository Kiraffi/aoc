
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

int64_t parseNumber(const char **ptr, bool gotoNext = true)
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
	if(*p != '\0' && gotoNext)
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

int readValues(const char *filename, std::vector<std::string> &outStrs)
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
		outStrs.push_back(s);
	}
	f.close();
	return 0;
}

int64_t parsePart(const char *start, const char *end, bool partB)
{
	int op = 0;
	int64_t result = 0;
	if(intptr_t(end) - intptr_t(start) < 0)
		return result;
	while(intptr_t(end) - intptr_t(start) >= 0)
	{
		switch(*start)
		{
			case '(':
			{
				int parenthesis = 1;
				const char *endTo = start + 1;
				while(parenthesis != 0 && intptr_t(end) - intptr_t(endTo) >= 0)
				{
					if(*endTo == ')') --parenthesis;
					if(*endTo == '(') ++parenthesis;
					++endTo;
				}
				int64_t value = parsePart(start + 1, endTo - 1, partB);
				if(op == 0) result = value;
				if(op == 1) result += value;
				if(op == 2) result *= value;

				op = 0;
				start = endTo;

				break;
			}
			case '+':
			{
				op = 1;
				break;
			}
			case '*':
			{
				if(partB)
				{
					result *= parsePart(start + 1, end, partB);
					start = end;
				}
				else
				{
					op = 2;
				}
				
				break;
			}
			default:
			{
				if(isdigit(*start))
				{
					int64_t value = parseNumber(&start, false);
					if(op == 0) result = value;
					if(op == 1) result += value;
					if(op == 2) result *= value;
					op = 0;
				}
				break;
			}

		}
		++start;
	}
	return result;
}


int main(int argc, char** argv)
{
	std::vector<std::string> values;

	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	int64_t sum = 0;
	for(const std::string &s : values)
	{
		int64_t value = parsePart(&s[0], &s[s.length() - 1], false);
		sum += value;
	}
	printf("Part a Sum: %" PRIi64 "\n", sum);

	sum = 0;
	for(const std::string &s : values)
	{
		int64_t value = parsePart(&s[0], &s[s.length() - 1], true); 
		sum += value;
	}
	printf("Part b Sum: %" PRIi64 "\n", sum);


	return 0;
}