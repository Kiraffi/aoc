
#include <algorithm>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <unordered_map>
#include <vector>

struct Item
{
	std::vector<std::string> compoundNames;
	std::vector<int> compoundAmounts;
	int createdAmount = 0;
	int64_t extraAmount = 0;
	int64_t producedAmount = 0;
};

int parseCount(const char **ch)
{
	const char *chh = ch[0];
	int count = 0;
	while(isdigit(*chh))
		count = count * 10 + (*chh++) - '0';
	ch[0] = chh;
	return count;
}

std::string parseName(const char **ch)
{
	std::string name;
	const char *chh = ch[0];
	while(isalpha(*chh))
			name += *chh++;
	ch[0] = chh;
	return name;
}

bool readValues(const char *filename, std::unordered_map<std::string, Item> &outItems)
{
	int count = 0;
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
		bool eq = false;
		Item item;
		while(!eq)
		{
			item.compoundAmounts.push_back(parseCount(&ptr));
			++ptr;
			item.compoundNames.push_back(parseName(&ptr));
			if(*ptr == ' ')
			{
				ptr += 4;
				eq = true;
			}
			else
			{
				ptr += 2;
			}
		}
		item.createdAmount = parseCount(&ptr);
		++ptr;
		outItems.insert({parseName(&ptr), item});
	}
	f.close();
	
	return true;
}

int comp(int a, int b)
{
	if(a < b)
		return 1;
	else if(a > b)
		return -1;
	return 0;
}

int abss(int a)
{
	return a > 0 ? a : -a;
}

int64_t maxx(int64_t a, int64_t b)
{
	return a > b ? a : b;
}

void doItem(Item &item, int64_t reqAmnt, std::unordered_map<std::string, Item> &items)
{
	int64_t multiplier = (reqAmnt + item.createdAmount - 1 - item.extraAmount) / item.createdAmount;
	item.extraAmount += multiplier * item.createdAmount - reqAmnt;
	item.producedAmount += multiplier * item.createdAmount;
	for(int i = 0; i < item.compoundNames.size(); ++i)
	{
		Item &chItem = items[item.compoundNames[i]];
		int64_t subreq = item.compoundAmounts[i] * multiplier;
		doItem(chItem, subreq, items);
	}
}

int main(int argc, char** argv)
{
	std::unordered_map<std::string, Item> items;
	if(!readValues("data.txt", items))
	{
		printf("Failed to load map!");
		return 0;
	};
	items["ORE"].createdAmount = 1;
	
	std::unordered_map<std::string, Item> copyItems = items;
	doItem(copyItems["FUEL"], 1, copyItems);

	int64_t originalOrePartA = copyItems["ORE"].producedAmount;
	printf("One FUEL requires: %" PRId64 " OREs\n", originalOrePartA);

	int64_t minFuel = 1;
	int64_t maxFuel = (2000000000000 + originalOrePartA - 1) / originalOrePartA;

	// Just doing binary search for optimal answer.
	while(minFuel + 1< maxFuel)
	{
		copyItems = items;
		int64_t midFuel = (minFuel + maxFuel) /2;
		doItem(copyItems["FUEL"], (minFuel + maxFuel) /2, copyItems);
		int64_t value = copyItems["ORE"].producedAmount; 
		if(value - int64_t(1000000000000) > 0)
		{
			maxFuel = midFuel;
		} 
		else
		{
			minFuel = midFuel;
		}
	}

	printf("Trillion ORE produces: %" PRId64 " FUEL\n", minFuel);

	return 0;
}