
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Compound
{
	std::vector<std::string> parents;
	std::vector<std::string> names;
	std::vector<int> counts;
};

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

int parseNumber(const char **ptr)
{
	const char *&p = ptr[0];
	int number = 0;
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

int readValues(const char *filename, std::unordered_map<std::string, Compound> &outCompounds)
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
		std::string name = parseName(&ptr);
		Compound &compound = outCompounds[name];
		findSpace(&ptr);
		findSpace(&ptr);
		while(*ptr != '\0')
		{
			compound.counts.push_back(parseNumber(&ptr));
			std::string childName = parseName(&ptr);
			compound.names.push_back(childName);
			outCompounds[childName].parents.push_back(name);
			findSpace(&ptr);
		}
	}
	f.close();
	return 0;
}

int countChildren(const std::string &name, std::unordered_map<std::string, Compound> &compounds)
{
	const Compound &compound = compounds[name];

	int sum = 0;
	for(int i = 0; i < compound.counts.size(); ++i)
	{
		int multi = compound.counts[i];
		sum += multi + multi * countChildren(compound.names[i], compounds);
	}
	return sum;
}

int main(int argc, char** argv)
{
	std::unordered_map<std::string, Compound> compounds;
	if(readValues("data.txt", compounds) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	std::unordered_set<std::string> foundParents;
	static const std::string searchName = "shiny gold";
	std::vector<std::string> searches = { searchName };

	while(searches.size() > 0)
	{
		std::string firstName = searches[0];
		if(firstName != searchName)
		{
			foundParents.insert(firstName);
		}
		for(const auto parent : compounds[firstName].parents)
			searches.push_back(parent);
		searches.erase(searches.begin());
	}
	printf("%s has %i holders bags\n", searchName.c_str(), foundParents.size());

	printf("%s holds %i bags in\n", searchName.c_str(), countChildren(searchName, compounds));


	return 0;
}