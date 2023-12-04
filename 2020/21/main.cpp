
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <iostream>
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

std::string parseWord(const char **ptr)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p && *p != ' ')
	{
		name += *p++;
	}
	if(*p != '\0') ++p;

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


int main(int argc, char** argv)
{
	std::vector<std::string> values;

	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	std::unordered_map<std::string, std::unordered_set<std::string>> wordsWithContains;
	std::unordered_map<std::string, int> allWords;


	for(const auto &s : values)
	{
		const char *ptr = s.data();
		static const char *containsStr = "(contains";
		std::string word = parseWord(&ptr);
		
		std::vector<std::string> words;
		while(strcmp(word.c_str(), containsStr) != 0)
		{
			allWords[word]++;
			words.push_back(word);
			word = parseWord(&ptr);
		}

		std::vector<std::string> containWords;
		while(*ptr != '\0')
		{
			word = parseWord(&ptr);
			word.erase(word.end() - 1);
			
			auto iter = wordsWithContains.find(word); 
			if(iter == wordsWithContains.end())
			{
				for(const auto &s2 : words)
					wordsWithContains[word].insert(s2);
			}
			else
			{
				std::vector<std::string> removeWords;
				for(const auto &s2 : iter->second)
				{
					auto iter2 = std::find(words.begin(), words.end(), s2);
					if(iter2 == words.end())
					{
						removeWords.push_back(s2);
					}
				}
				for(const auto &s2 : removeWords)
				{
					iter->second.erase(s2);
				}
			}
			
			containWords.push_back(word);
		}
	}
	// Remove all words that do contain in somewhere
	for(const auto &s : wordsWithContains)
	{
		for(const auto &s2 : s.second)
		{
			allWords.erase(s2);
		}
	}

	int64_t unseenCount = 0;
	for(const auto &s : allWords)
	{
		unseenCount += s.second;
		// Remove words that do not belong into any allergy from allergylist
		for(auto &t : wordsWithContains)
		{
			t.second.erase(s.first);
		}
	}

	printf("Part A: unseen allergies %" PRIi64 "\n", unseenCount);


	bool erasing = true;
	while(erasing)
	{
		erasing = false;
		for(auto &p : wordsWithContains)
		{
			assert(p.second.size() > 0);
			if(p.second.size() == 1)
			{
				const std::string &removeWord = (*p.second.begin());

				for(auto &q : wordsWithContains)
				{
					if(q.second.size() > 1)
					{
						erasing |= (q.second.erase(removeWord)) > 0;
					}
				}
			}
		}
	}

	struct ContainMeaning
	{
		std::string englishWord;
		std::string alienWord;
		static bool comp(const ContainMeaning &a, const ContainMeaning &b)
		{
			return a.englishWord < b.englishWord;
		}
	};

	std::vector<ContainMeaning> meanings;

	for(auto &p : wordsWithContains)
	{
		assert(p.second.size() == 1);
		meanings.push_back({p.first, (*p.second.begin())});
	}
	std::string finalStr;
	std::sort(meanings.begin(), meanings.end(), ContainMeaning::comp);
	for(auto &s: meanings)
	{
		finalStr += s.alienWord + ",";
	}
	finalStr.erase(finalStr.end() - 1);
	printf("Part B: %s\n", finalStr.c_str());
	return 0;
}