
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

struct WirePart
{
	int length;
	int xDirection;
	int yDirection;
};

struct WireMap
{
	int xPosSize;
	int yPosSize;

	int xNegSize;
	int yNegSize;

	
	std::vector<WirePart> parts;
};


bool readValues(const char *filename, std::vector<WireMap> &outValues)
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
		
		outValues.push_back(WireMap {0, 0, 0, 0, {}});
		WireMap &m = outValues[outValues.size() - 1];
		int v = 0;
		int xdir = 0;
		int ydir = 0;
		int xpos = 0;
		int ypos = 0;
		for(int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if(c >= '0' && c <= '9')
			{
				v = v * 10 + (c - '0');
			}
			if(c == ',' || i == s.length() - 1)
			{
				ypos += ydir * v;
				xpos += xdir * v;

				m.xPosSize = maxx(m.xPosSize, xpos + 1);
				m.yPosSize = maxx(m.yPosSize, ypos + 1);
				m.xNegSize = minn(m.xNegSize, xpos - 1);
				m.yNegSize = minn(m.yNegSize, ypos - 1);
				m.parts.emplace_back(WirePart{v, xdir, ydir});
				v = 0;
				xdir = 0;
				ydir = 0;
			}
			if(c == 'D')
			{
				ydir = 1;
			}
			if( c == 'U')
			{
				ydir = -1;
			}
			if( c == 'L')
			{
				xdir = -1;
			}
			if( c == 'R')
			{
				xdir = 1;
			}
			
		}
	}
	f.close();
	
	return true;
}


void printMap(std::vector<std::vector<char>> &map)
{
	for(int j = 0; j < map.size(); ++j)
	{
		for (int i = 0; i < map[j].size(); ++i)
		{
			printf("%c", map[j][i]);
		}
		printf("\n");
	}
}

struct ResultStruct
{
	int manhattanDistance = MAXIM_INT;
	int wireDistance = MAXIM_INT;
};


ResultStruct fillmap(std::vector<std::vector<int>> &map, const WireMap &wiremap, int minX, int minY, bool checkIntersection)
{
	int xpos = -minX;
	int ypos = -minY;

	ResultStruct resultStruct;
	int steps = 0;

	printf("start pos: %i, %i\n", xpos, ypos);
	map[ypos][xpos] = 0;
	for(const WirePart &part : wiremap.parts)
	{
		for(int i = 0; i < part.length; ++i)
		{
			xpos += part.xDirection;
			ypos += part.yDirection;
			++steps;
			if(checkIntersection && map[ypos][xpos] != MAXIM_INT)
			{
				// crash....
				resultStruct.manhattanDistance = minn(abss(xpos + minX) + abss(ypos + minY), resultStruct.manhattanDistance);
				resultStruct.wireDistance = minn(steps + map[ypos][xpos], resultStruct.wireDistance);
			}
			else if(!checkIntersection)
			{
				map[ypos][xpos] = minn(map[ypos][xpos], steps);
			}
		}
	}
	return resultStruct;
}

int main(int argc, char** argv)
{
	std::vector<WireMap> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}

	int minX = 0;
	int minY = 0;
	int maxX = 0;
	int maxY = 0;

	for(int i = 0; i < values.size(); ++i)
	{
		minX = minn(minX, values[i].xNegSize);
		minY = minn(minY, values[i].yNegSize);
		maxX = maxx(maxX, values[i].xPosSize);
		maxY = maxx(maxY, values[i].yPosSize);
	}
	printf("sizes: %i, %i:%i, %i\n", minX, minY, maxX, maxY);

	std::vector<std::vector<int>> map;
	int width =  maxX - minX;
	int height = maxY - minY;

	printf("map size: %i, %i\n", width, height);
	map.resize(height, std::vector<int>(width, MAXIM_INT));

	fillmap(map, values[0], minX, minY, false);

	ResultStruct result = fillmap(map, values[1], minX, minY, true);
	printf("Shortest manhattan distance: %i\n", result.manhattanDistance);
	printf("Shortest wire distance: %i\n", result.wireDistance);
	//printMap(map);

	return 0;
}