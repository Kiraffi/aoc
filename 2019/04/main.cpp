
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

static constexpr int MAXIM_INT = 0x7fffffff;

int maxx(int a, int b)
{
	if(a > b)
		return a;
	return b;
}

int minn(int a, int b)
{
	if(a < b)
		return a;
	return b;
}

int abss(int a)
{
	return a < 0 ? -a : a;
}


bool readValues(const char *filename, std::vector<int> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	bool findMin = true;
	while(getline(f, s))
	{
		int v = 0;
		int lastDigit = 0;
		for(int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if(c >= '0' && c <= '9')
			{
				lastDigit = maxx(lastDigit, c - '0');
				v = v * 10 + c - '0';
			}
			if(c == '-')
			{
				outValues.emplace_back(v);
				v = 0;
			}
		}
		outValues.emplace_back(v);
	}
	f.close();
	
	return true;
}

int countCombos(int start, int end, int currValue, int prevNumber, int index, int maxComboNumbers)
{
	if(currValue > end || index < 0)
		return 0;

	int minNumber = 0;
	int m = 1;
	for(int i = 0; i < index; ++i)
		m *= 10;
	
	int startN = prevNumber;
	int count = 0;

	if( index == 0)
	{
		for(;startN <= 9; ++startN)
		{
			int v = currValue + startN * m;
			if(v <= end && v >= startN)
			{
				bool found = false;
				int combos = 0;
				int tmpV = v;
				int lastDigit = tmpV % 10;
				while(tmpV / 10 > 0)
				{
					tmpV /= 10;
					int newDigit = tmpV % 10;
					if(newDigit == lastDigit)
					{
						++combos;
					}
					else
					{
						if(combos < maxComboNumbers && combos > 0)
						{
							found = true;
						}
						combos = 0;
					}
					
					lastDigit = newDigit;
				}
				if((combos < maxComboNumbers && combos > 0) || found)
				{
					printf("%i\n", v);
					++count;
				}
			}
		}
	}
	else
	{
		for(;startN <= 9; ++startN)
		{
			if(currValue + (startN + 1) * m > start)
				count += countCombos(start, end, currValue + startN * m, startN, index - 1, maxComboNumbers);
		}
	}
	return count;	
}

int main(int argc, char** argv)
{
	std::vector<int> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 2)
	{
		printf("Data should have at least 2 number!\n");
		return 0;
	}

	for(int i = 0; i < values.size(); ++i)
	{
		printf("value: %i\n", values[i]);
	}
	int startNumber = values[0];
	while(startNumber / 10 > 0)
		startNumber /= 10;
	printf("values: %i\n", countCombos(values[0], values[1], 0, startNumber, 5, 6));
	printf("values: %i\n", countCombos(values[0], values[1], 0, startNumber, 5, 2));

	return 0;
}