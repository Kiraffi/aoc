
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
struct Rule
{
	std::string value;
	std::vector<std::vector<int>> lines;
};


std::vector<std::string> parsePossibleStrings(const std::vector<Rule> &rules, int level)
{
	std::vector<std::string> result;
	const Rule &rule = rules[level];
	if(rule.value.length() > 0)
	{
		result.push_back(rule.value);
	}

	for(const auto &a : rule.lines)
	{
		std::vector<std::string> result2 (1, "");
		for(const auto &b : a)
		{
			int count = result2.size();
			std::vector<std::string> result4 = result2;
			std::vector<std::string> result3 = parsePossibleStrings(rules, b);
			result2.clear();
			for(int j = 0; j < count; ++j)
			{
				for(int i = 0; i < result3.size(); ++i)
				{
					result2.push_back(result4[j] + result3[i]);
				}
			}
		}
		result.insert(result.end(), result2.begin(), result2.end());
	}
	return result;
}

int findMatchingOffset(const std::string &findStr, const std::vector<std::string> &possibleStrings,
	int startOffset)
{
	for(const auto &s2 : possibleStrings)
	{
		if(strncmp(findStr.c_str() + startOffset, s2.c_str(), s2.size()) == 0)
		{
			return startOffset + s2.length();
		}
	}
	return -1;
}

void printMatches(const std::vector<std::string> &possibleStrings1,
	const std::vector<std::string> &possibleStrings2,
	const std::vector<std::string> &values, bool partA)
{
	int validStrs = 0;

	for(const auto &s : values)
	{
		int offset = 0;
		int lefts = 0;
		int rights = 0;
		while(offset < s.length())
		{
			int newOffset = findMatchingOffset(s, possibleStrings1, offset);
			if(newOffset == -1)
				break;

			++lefts;
			offset = newOffset;
		}
		while(offset < s.length())
		{
			int newOffset = findMatchingOffset(s, possibleStrings2, offset);
			if(newOffset == -1)
				break;

			++rights;
			offset = newOffset;
		}

		if(partA && lefts == 2 && rights == 1 && offset == s.length())
		{
			validStrs++;
		}
		else if(!partA && lefts > rights && rights >= 1 && offset == s.length())
		{
			validStrs++;
		}
	}
	if(partA)
		printf("Part A: valid strings: %i\n", validStrs);
	else
		printf("Part B: valid strings: %i\n", validStrs);
}


int main(int argc, char** argv)
{
	std::vector<std::string> values;

	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};


	std::vector<Rule> rules(255);

	int lineN = 0;
	// Parse rules
	while(values[lineN].length() > 0)
	{
		const char *str = values[lineN].data();
		int lineNumber = parseNumber(&str);
		Rule &rule = rules[lineNumber];
		str += 1;
		int ruleNumber = 0;
		while(*str != '\0')
		{
			if(isdigit(*str))
			{
				if(ruleNumber >= rule.lines.size())
					rule.lines.push_back(std::vector<int>());
				
				rule.lines[ruleNumber].push_back(parseNumber(&str));
			}
			else if(*str == '|')
			{
				++ruleNumber;
				++str;
			}
			else if(*str == '\"')
			{
				++str;
				while(*str != '\"') rule.value += *str++;
				if(*str != '\0')
					++str;
			}
			else
				++str;
		}
		++lineN;
	}
	values.erase(values.begin(), values.begin() + lineN);
	std::vector<std::string> possibleStrings1 = parsePossibleStrings(rules, 42);
	std::vector<std::string> possibleStrings2 = parsePossibleStrings(rules, 31);

	struct Comp { static bool comp(const std::string &a, const std::string &b) { return a < b;} };
	// There is no matching string with possibleStrings1 and possibleStrings2
	std::sort(possibleStrings1.begin(), possibleStrings1.end(), Comp::comp);
	std::sort(possibleStrings2.begin(), possibleStrings2.end(), Comp::comp);

	printMatches(possibleStrings1, possibleStrings2, values, true);
	printMatches(possibleStrings1, possibleStrings2, values, false);
	return 0;
}