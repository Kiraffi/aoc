
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>


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

int readValues(const char *filename, std::vector<std::string> &outLines)
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
		outLines.push_back(s);
	}
	f.close();
	return 0;
}

int isOccupied(const std::vector<std::string> &lines, int x, int y)
{
	if(x < 0 || y < 0 || y >= lines.size() || x >= lines[y].size())
		return -2;
	
	int result = 0;
	if(lines[y][x] == '.')
		result = -1;
	if(lines[y][x] == 'L')
		result = 0;
	if(lines[y][x] == '#')
		result = 1;
	return result;
}

void partA(std::vector<std::string> lines)
{
	bool changed = true;
	int rounds = 0;
	while(changed)
	{
		changed = false;
		std::vector<std::string> newLines = lines;
		for(int j = 0; j < lines.size(); ++j)
		{
			for(int i = 0; i < lines[j].size(); ++i)
			{
				int adjacents = 0;
				for(int j2 = j - 1; j2 <= j + 1; ++j2)
				{
					for(int i2 = i - 1; i2 <= i + 1; ++i2)
					{
						if(i2 == i && j2 == j)
							continue;
						
						adjacents += (isOccupied(lines, i2, j2)) == 1;
					}
				}
				if(isOccupied(lines, i, j) == 1 && adjacents >= 4)
				{
					changed = true;
					newLines[j][i] = 'L';
				}
				else if(isOccupied(lines, i, j) == 0 && adjacents == 0)
				{
					changed = true;
					newLines[j][i] = '#';
				}
			}
		}
		lines = newLines;
		++rounds;
	}
	int occupied = 0;
	for(int j = 0; j < lines.size(); ++j)
	{
		for(int i = 0; i < lines[j].size(); ++i)
		{
			occupied += isOccupied(lines, i, j) == 1;
		}
	}
	printf("Rounds: %i, occupied: %i\n", rounds, occupied);

}

void partB(std::vector<std::string> lines)
{
	bool changed = true;
	int rounds = 0;
	while(changed)
	{
		changed = false;
		std::vector<std::string> newLines = lines;
		for(int j = 0; j < lines.size(); ++j)
		{
			for(int i = 0; i < lines[j].size(); ++i)
			{
				int adjacents = 0;
				for(int j2 = -1; j2 <= 1; ++j2)
				{
					for(int i2 = -1; i2 <= 1; ++i2)
					{
						if(i2 == 0 && j2 == 0)
							continue;
						
						int distance = 1;
						int result = -1;
						while(result == -1)
						{
							result = isOccupied(lines, i + i2 * distance, j + j2* distance);
							++distance;
						}
						adjacents += result == 1;
					}
				}
				if(isOccupied(lines, i, j) == 1 && adjacents >= 5)
				{
					changed = true;
					newLines[j][i] = 'L';
				}
				else if(isOccupied(lines, i, j) == 0 && adjacents == 0)
				{
					changed = true;
					newLines[j][i] = '#';
				}
			}
		}
		lines = newLines;
		++rounds;
	}
	int occupied = 0;
	for(int j = 0; j < lines.size(); ++j)
	{
		for(int i = 0; i < lines[j].size(); ++i)
		{
			occupied += isOccupied(lines, i, j) == 1;
		}
	}
	printf("Rounds: %i, occupied: %i\n", rounds, occupied);

}

int main(int argc, char** argv)
{
	std::vector<std::string> lines;
	if(readValues("data.txt", lines) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	printf("Part a:");
	partA(lines);

	printf("Part b:");
	partB(lines);
	return 0;
}