
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

int readValues(const char *filename, std::vector<std::string> &outMap)
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
		outMap.push_back(s);
	}
	f.close();
	
	return true;
}

struct Slope
{
	int x;
	int y;
};


int countHits(const Slope &slope, const std::vector<std::string> &map)
{
	int width = map[0].size();
	int x = slope.x;
	int y = slope.y;
	int trees = 0;

	while(y < map.size())
	{
		if(map[y][x] == '#')
			++trees;
		x = (x + slope.x) % width;
		y += slope.y;
	}

	return trees;
}

int main(int argc, char** argv)
{
	std::vector<std::string> map;
	if(!readValues("data.txt", map))
	{
		printf("Failed to load map!");
		return 0;
	};

	int trees = countHits(Slope{3, 1}, map);
	printf("Trees hit for slope {3,1}: %i\n", trees);


	Slope slopes[5] = {
		{1, 1},
		{3, 1},
		{5, 1},
		{7, 1},
		{1, 2}
	};

	int64_t multi = 1;

	for(const Slope &slope : slopes)
	{
		int ans = countHits(slope, map);
		multi *= ans;
	}
	printf("Tree hits multiplied: %" PRId64 "\n", multi);

	return 0;
}