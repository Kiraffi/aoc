
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

int readValues(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}
	std::string s;

	int numbers[10] = {};

	int maxZeros = (1 << 30);
	int multi = 0;

	std::vector<int> image;
	image.resize(25 * 6, 2);

	int index = 0;

	while(getline(f, s))
	{
		for(int i = 0; i < s.size(); ++i)
		{
			int v = int(s[i]) - '0';

			++numbers[v];
			if(v < 2 && image[index] > 1)
			{
				image[index] = v;
			}
			++index;
			if(index >= 25 * 6)
			{
				if(maxZeros > numbers[0])
				{
					maxZeros = numbers[0];
					multi = numbers[1] * numbers[2];
				}

				for(int i = 0; i < 10; ++i)
				{
					numbers[i] = 0;
				}
				index = 0;
			}
		}
	}
	f.close();

	printf("Max zeros: %i, multi: %i\n", maxZeros, multi);

	for(int y = 0; y < 6; ++y)
	{
		for(int x = 0; x < 25; ++x)
		{
			int i = image[x + y * 25];
			if(i == 1)
				printf("X");
			else 
				printf(" ");
		}
		printf("\n");
	}

}

int main(int argc, char** argv)
{
	readValues("data.txt");
	return 0;
}