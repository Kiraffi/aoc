
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

std::vector<char> parseCards(const std::vector<std::string> &values, int &valuesCount)
{
	std::vector<char> result;
	while(true)
	{
		const std::string &s = values[valuesCount];
		if(s.length() == 0)
			return result;
		const char *ptr = s.data();
		result.push_back(parseNumber(&ptr));
		++valuesCount;
	}
	return result;
}

struct Decks
{
	std::vector<char> player1Cards;
	std::vector<char> player2Cards;
};


int64_t calculateScore(const std::vector<char> &winnerDeck)
{
	int64_t multip = 0;
	for(int i = winnerDeck.size() - 1; i >= 0; --i)
		multip += (winnerDeck.size() - i) * winnerDeck[i];
	return multip;
}


void partA(const std::vector<std::string> &values)
{
	int valuesCount = 1;
	std::vector<char> p1Cards = parseCards(values, valuesCount);
	valuesCount += 2;
	std::vector<char> p2Cards = parseCards(values, valuesCount);

	Decks deck = { p1Cards, p2Cards };

	while(deck.player1Cards.size() > 0 && deck.player2Cards.size() > 0)
	{
		int p1Card = deck.player1Cards[0];
		int p2Card = deck.player2Cards[0];
		deck.player1Cards.erase(deck.player1Cards.begin());
		deck.player2Cards.erase(deck.player2Cards.begin());
		bool p1Winner = false;
		if(p1Card > p2Card)
		{
			p1Winner = true;
		}
		else
		{
			p1Winner = false;
		}
		if(p1Winner)
		{
			deck.player1Cards.push_back(p1Card);
			deck.player1Cards.push_back(p2Card);
		}
		else
		{
			deck.player2Cards.push_back(p2Card);
			deck.player2Cards.push_back(p1Card);
		}
	}

	std::vector<char> *winnerCards = &deck.player1Cards;
	if(deck.player2Cards.size() > 0) 
		winnerCards = &deck.player2Cards;
	int64_t multip = calculateScore((*winnerCards));

	printf("Part a: winning score: %" PRIi64 "\n", multip);
}

bool playRecursive(Decks &deck)
{
	std::unordered_map<uint64_t, std::vector<Decks>> hashDecks;
	while(deck.player1Cards.size() > 0 && deck.player2Cards.size() > 0)
	{
		uint64_t hashIndex = calculateScore(deck.player1Cards);
		hashIndex *= calculateScore(deck.player2Cards);

		std::vector<Decks> &seenDecks = hashDecks[hashIndex];
		for(const auto &compDeck : seenDecks)
		{
			if(memcmp(deck.player1Cards.data(), compDeck.player1Cards.data(), deck.player1Cards.size()) == 0 &&
				memcmp(deck.player2Cards.data(), compDeck.player2Cards.data(), deck.player2Cards.size()) == 0)
			{
				return true;
			}
		}
		seenDecks.push_back(deck);

		int p1Card = deck.player1Cards[0];
		int p2Card = deck.player2Cards[0];
		deck.player1Cards.erase(deck.player1Cards.begin());
		deck.player2Cards.erase(deck.player2Cards.begin());
		bool p1Winner = false;
		if(p1Card <= deck.player1Cards.size() && p2Card <= deck.player2Cards.size())
		{
			Decks deck2;
			deck2.player1Cards = 
				std::vector<char>(deck.player1Cards.begin(), deck.player1Cards.begin() + p1Card);
			deck2.player2Cards = 
				std::vector<char>(deck.player2Cards.begin(), deck.player2Cards.begin() + p2Card);
			p1Winner = playRecursive(deck2);
		}
		else if(p1Card > p2Card)
		{
			p1Winner = true;
		}
		else
		{
			p1Winner = false;
		}
		if(p1Winner)
		{
			deck.player1Cards.push_back(p1Card);
			deck.player1Cards.push_back(p2Card);
		}
		else
		{
			deck.player2Cards.push_back(p2Card);
			deck.player2Cards.push_back(p1Card);
		}
	}
	return deck.player1Cards.size() > deck.player2Cards.size();
}


void partB(const std::vector<std::string> &values)
{
	int valuesCount = 1;
	std::vector<char> p1Cards = parseCards(values, valuesCount);
	valuesCount += 2;
	std::vector<char> p2Cards = parseCards(values, valuesCount);
	Decks deck{p1Cards, p2Cards};

	playRecursive(deck);

	std::vector<char> *winnerCards = &deck.player1Cards;
	if(deck.player2Cards.size() > 0) 
		winnerCards = &deck.player2Cards;
	int64_t multip = calculateScore((*winnerCards));

	printf("Part b: winning score: %" PRIi64 "\n", multip);
}


int main(int argc, char** argv)
{
	std::vector<std::string> values;

	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	partA(values);
	partB(values);

	return 0;
}