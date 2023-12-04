
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

int readValues(const char *filename, std::vector<std::vector<int>> &outGroupValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}
	std::string s;

	outGroupValues.push_back(std::vector<int>());
	int groupIndex = 0; 

	while(getline(f, s))
	{
		if(s.empty())
		{
			++groupIndex;
			outGroupValues.push_back(std::vector<int>());
		}
		else
		{
			int lineValues = 0;
			for(char c : s)
			{
				lineValues |= 1 << (c - 'a');
			}
			outGroupValues[groupIndex].push_back(lineValues);
		}
	}
	f.close();
	return 0;
}


int main(int argc, char** argv)
{
	std::vector<std::vector<int>> values;
	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	int sum = 0;
	int sumand = 0;
	for(auto &v : values)
	{
		int sumGroupAll = 0;
		int sumGroupAllAnd = ~0;
		for(int v2 : v)
		{
			sumGroupAll |= v2;
			sumGroupAllAnd &= v2;
		}

		while(sumGroupAll > 0)
		{
			sum += sumGroupAll & 1;
			sumGroupAll = sumGroupAll >> 1;
		}

		while(sumGroupAllAnd > 0)
		{
			sumand += sumGroupAllAnd & 1;
			sumGroupAllAnd = sumGroupAllAnd >> 1;
		}
	}

	printf("Sum of all yesses: %i\n", sum);
	printf("Sum of all group yesses: %i\n", sumand);

	return 0;
}