
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>


struct Map
{
	std::vector<std::vector<std::string>> map;
	int sizeX;
	int sizeY;
	int sizeZ;
};

struct Map2
{
	std::vector<std::vector<std::vector<std::string>>> map;
	int sizeX;
	int sizeY;
	int sizeZ;
	int sizeW;
};

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

int64_t parseNumber(const char **ptr)
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
	if(*p != '\0')
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

bool isActive(int x, int y, int z, const Map &map)
{
	if(x < 0 || y < 0 || z < 0 ||
		x >= map.sizeX || y >= map.sizeY || z >= map.sizeZ)
	{
		return false;
	}
	return map.map[z][y][x] == '#';

}

bool isActive(int x, int y, int z, int w, const Map2 &map)
{
	if(x < 0 || y < 0 || z < 0 || w < 0 ||
		x >= map.sizeX || y >= map.sizeY || z >= map.sizeZ || w >= map.sizeW)
	{
		return false;
	}
	return map.map[w][z][y][x] == '#';

}


int countNeighbours(int x, int y, int z, const Map &map)
{
	int result = 0;
	for(int k = -1; k <= 1; ++k)
	{
		for(int j = -1; j <= 1; ++j)
		{
			for(int i = -1; i <= 1; ++i)
			{
				if(i == 0 && j == 0 && k == 0)
					continue;
				if(isActive(x + i, y + j, z + k, map))
					++result;
			}
			
		}
	}
	return result;
}

int countNeighbours(int x, int y, int z, int w, const Map2 &map)
{
	int result = 0;
	for(int l = -1; l <= 1; ++l)
	{
		for(int k = -1; k <= 1; ++k)
		{
			for(int j = -1; j <= 1; ++j)
			{
				for(int i = -1; i <= 1; ++i)
				{
					if(i == 0 && j == 0 && k == 0 && l == 0)
						continue;
					if(isActive(x + i, y + j, z + k, w + l, map))
						++result;
				}
				
			}
		}
	}
	return result;
}

void partB(const std::vector<std::string> &values)
{
	Map2 map;	
	map.map.push_back(std::vector<std::vector<std::string>> (1, values));
	map.sizeW = 1;
	map.sizeZ = 1;
	map.sizeY = values.size();
	map.sizeX = values[0].size();

	Map2 newMap;

	for(int laps = 0; laps < 6; ++laps)
	{
		newMap.map = std::vector<std::vector<std::vector<std::string>>>(map.sizeW + 2, 
			std::vector<std::vector<std::string>>(map.sizeZ + 2,
				std::vector<std::string>(map.sizeY + 2,
					std::string(map.sizeX + 2, '.'))));
		newMap.sizeW = map.sizeW + 2;
		newMap.sizeZ = map.sizeZ + 2;
		newMap.sizeY = map.sizeY + 2;
		newMap.sizeX = map.sizeX + 2;

		for(int w = -1; w <= map.sizeW; ++w)
		{
			for(int z = -1; z <= map.sizeZ; ++z)
			{
				for(int y = -1; y <= map.sizeY; ++y)
				{
					for(int x = -1; x <= map.sizeX; ++x)
					{
						int c = countNeighbours(x, y, z, w, map);
						bool active = isActive(x, y, z, w, map);
						char d = '.';
						if(c == 3 || (active && c == 2))
						{
							d = '#';
						}
						newMap.map[w + 1][z + 1][y + 1][x + 1] = d;
					}
				}
			}
		}
		map = newMap;
	}
	
	int64_t count = 0;
	for(const auto &a : newMap.map)
	{
		for(const auto &b : a)
			for(const auto &c : b)
				for(const auto &d : c)
					count += d == '#';
	}
	printf("Part b: Count: %" PRIi64 "\n", count);

}

void partA(const std::vector<std::string> &values)
{
	Map map;
	map.map.push_back(values);
	map.sizeZ = 1;
	map.sizeY = values.size();
	map.sizeX = values[0].size();

	Map newMap;

	for(int laps = 0; laps < 6; ++laps)
	{
		newMap.map = std::vector<std::vector<std::string>>(map.sizeZ + 2,
			std::vector<std::string>(map.sizeY + 2,
				std::string(map.sizeX + 2, '.')));
		newMap.sizeZ = map.sizeZ + 2;
		newMap.sizeY = map.sizeY + 2;
		newMap.sizeX = map.sizeX + 2;

		for(int z = -1; z <= map.sizeZ; ++z)
		{
			for(int y = -1; y <= map.sizeY; ++y)
			{
				for(int x = -1; x <= map.sizeX; ++x)
				{
					int c = countNeighbours(x, y, z, map);
					bool active = isActive(x, y, z, map);
					char d = '.';
					if(c == 3 || (active && c == 2))
					{
						d = '#';
					}
					newMap.map[z + 1][y + 1][x + 1] = d;
				}
			}
		}
		map = newMap;
	}
	
	int64_t count = 0;
	for(const auto &a : newMap.map)
	{
		for(const auto &b : a)
			for(const auto &c : b)
				count += c == '#';
	}
	printf("Part a: Count: %" PRIi64 "\n", count);

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