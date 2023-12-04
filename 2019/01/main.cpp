
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

bool readValues(const char *filename, std::vector<int> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	while(getline(f, s))
	{
		outValues.emplace_back(std::stoi(s));
	}
	f.close();
	
	return true;
}

void first(const std::vector<int> &values)
{
	int64_t sum = 0;
	for(int i = 0; i < values.size(); ++i)
	{
		int v = values[i] / 3 - 2;
		if(v > 0)
			sum += v;
	}

	printf("First sum of fuel: %" PRId64 "\n", sum);
}

void second(const std::vector<int> &values)
{
	int64_t sum = 0;
	for(int i = 0; i < values.size(); ++i)
	{
		int v = values[i];
		while(v > 0 )
		{
			v = v / 3 - 2;
			if(v > 0)
			{
				sum += v;
			}
		}
	}

	printf("Second sum of fuel: %" PRId64 "\n", sum);
}

int main(int argc, char** argv)
{
	std::vector<int> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}

	first(values);
	second(values);

	return 0;
}