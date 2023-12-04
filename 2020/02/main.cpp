
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

int readValues(const char *filename, bool partA)
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
		int minValue = 0;
		int maxValue = 0;
		int ind = 0;
		while(s[ind] != '-')
		{
			minValue = minValue * 10 + s[ind] - '0';
			++ind;
		}
		++ind;
		while(s[ind] != ' ')
		{
			maxValue = maxValue * 10 + s[ind] - '0';
			++ind;
		}
		char findChar = s[ind + 1];
		int startIndex = ind + 4;
		if(partA)
		{
			int occurences = 0; 
			for(int i = startIndex; i < s.size(); ++i)
			{
				if(s[i] == findChar)
					++occurences;
			}
			if(occurences >= minValue && occurences <= maxValue)
				++count;
		}
		else
		{
			minValue -= 1;
			maxValue -= 1;
			if(maxValue < s.size() - startIndex && 
				(s[startIndex + minValue] == findChar ^ s[startIndex + maxValue] == findChar))
			{
				++count;
			}
		}
		
	}
	f.close();
	
	return count;
}

int main(int argc, char** argv)
{
	printf("count: %i\n", readValues("data.txt", true));
	printf("count: %i\n", readValues("data.txt", false));
	return 0;
}