
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
		int v = 0;
		for(int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if(c >= '0' && c <= '9')
			{
				v = v * 10 + (c - '0');
			}
			if(c == ',' || i == s.length() - 1)
			{
				outValues.emplace_back(v);
				v = 0;
			}
			
		}
	}
	f.close();
	
	return true;
}

bool test(const std::vector<int> &values, const std::vector<int> &expected)
{
	if(values.size() != expected.size())
	{
		if(values.size() < expected.size())
			printf("Values have less numbers than expected\n");
		else
			printf("Expected has more numbers than values\n");
		return false;
	}

	for(int i = 0; i < values.size(); ++i)
	{
		if(values[i] != expected[i])
		{
			printf("Index: %i doesnt match! Value: %i, expected: %i\n", 
				i , values[i], expected[i]);
			return false;
		}
	}
	return true;
}

struct Values
{
	int l;
	int r;
	int out;
};

bool parseValues(const std::vector<int> &values, int pointer, Values &outValues)
{
	if(pointer + 4 >= values.size())
	{
		printf("Pointer out of range for parsing values. \n");
		return false;
	}

	int l = values[pointer + 1];
	int r = values[pointer + 2];
	int out = values[pointer + 3];
	//printf("r:%i + r:%i to %i\n", l, r, out);
	if(out >= values.size() || l >= values.size() || r >= values.size())
	{
		/*
		printf("trying to read/write out of position, reads: %i, %i, write: %i vs size: %i\n",
			l, r, out, int(values.size()));
		*/
		return false;
	}
	l = values[l];
	r = values[r];
	//printf("values: %i, %i to %i\n", l, r, out);

	outValues.l = l;
	outValues.r = r;
	outValues.out = out;

	return true;
}



void printValues(const std::vector<int> &values)
{
	for(int i = 0; i < values.size(); ++i)
	{
		if( i + 1 < values.size())
			printf("%i,", values[i]);
		else 
			printf("%i", values[i]);
	}
	printf("\n");

}

// pass by copy
bool program(std::vector<int> values, int replaceFirst, int replaceSecond)
{
	int pointer = 0;
	values[1] = replaceFirst;
	values[2] = replaceSecond;
	while(pointer < values.size())
	{
		switch(values[pointer])
		{
			case 1:
			{
				//printf("add op:");
				Values v;
				if(!parseValues(values, pointer, v))
				{
					return false;
				}

				values[v.out] = v.l + v.r;
			}
			break;

			case 2:
			{
				//printf("mul op:");
				Values v;
				if(!parseValues(values, pointer, v))
				{
					return false;
				}

				values[v.out] = v.l * v.r;
			}
			break;

			case 99:
			{
				printf("Halted\n");
				if(values[0] == 19690720)
				{
					printf("1: %i, 2: %i\n", values[1], values[2]);
					return true;
				}
				//printValues(values);
				return false;
			}
			break;

			default:
			{
				printf("No operation...\n");
				return false;
			}
			break;
		}
		pointer += 4;
	}

	printf("Trying to read op codes out of array: pointer: %i vs size: %i\n",
		pointer, int(values.size()));
	return false;
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

	program(values, 12, 2);

	for(int i = 0; i < 100; ++i)
	{
		for(int j = 0; j < 100; ++j)
		{
			if(program(values, i, j))
			{
				return 0;	
			}
		}

	}


	return 0;
}