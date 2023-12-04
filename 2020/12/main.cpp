
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
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


void partA(const std::vector<std::string> &lines)
{
	int angle = 90;
	int ypos = 0;
	int xpos = 0;
	int headingX = 1;
	int headingY = 0;

	for(const std::string &s : lines)
	{
		const char *n = &s[1];
		int number = parseNumber(&n);
		switch(s[0])
		{
			case 'N': { ypos += number; break; }
			case 'S': { ypos -= number; break; }
			case 'W': { xpos -= number; break; }
			case 'E': { xpos += number; break; }
			case 'L': { angle -= number; break; }
			case 'R': { angle += number; break; }
			case 'F': { xpos += headingX * number; ypos += headingY * number; break; }
		}
		while(angle < 0) angle += 360;
		angle = angle % 360;
		assert(angle % 90 == 0);
		headingX = (angle == 90) - 1* (angle == 270);
		headingY = (angle == 0) - 1* (angle == 180);
	}
	printf("Part a, manhattan distance: %i\n", abs(xpos) + abs(ypos));
}

int swap(int &a, int &b)
{
	int tmp = a;
	a = b;
	b = tmp;
}

void partB(const std::vector<std::string> &lines)
{
	int xShipPos = 0;
	int yShipPos = 0;


	int xWaypointPos = 10;
	int yWaypointPos = 1;

	for(const std::string &s : lines)
	{
		const char *n = &s[1];
		int number = parseNumber(&n);
		switch(s[0])
		{
			case 'N': { yWaypointPos += number; break; }
			case 'S': { yWaypointPos -= number; break; }
			case 'W': { xWaypointPos -= number; break; }
			case 'E': { xWaypointPos += number; break; }
			case 'L': 
			{ 
				while(number >= 90)
				{
					swap(xWaypointPos, yWaypointPos);
					xWaypointPos = -xWaypointPos;
					number -= 90;
				}
				break; 
			}
			case 'R': 
			{ 
				while(number >= 90)
				{
					swap(xWaypointPos, yWaypointPos);
					yWaypointPos = -yWaypointPos;
					number -= 90;
				}
				break; 
			}
			case 'F': { xShipPos += xWaypointPos * number; yShipPos += yWaypointPos * number; break; }
		}
	}
	printf("Part b, manhattan distance: %i\n", abs(xShipPos) + abs(yShipPos));
}


int main(int argc, char** argv)
{
	std::vector<std::string> lines;
	if(readValues("data.txt", lines) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	partA(lines);
	partB(lines);
	return 0;
}