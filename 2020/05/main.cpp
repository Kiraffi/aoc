
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>


struct ReturnValues 
{
	int highest = -1;
	int id = -1;
};

ReturnValues readValues(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return {-1, -1};
	}
	int highest = -1;
	std::vector<int> seatedSeats((1<<10), 0);
	std::string s;
	while(getline(f, s))
	{
		int pos = 0;
		for(int i = 0; i < 7; ++i)
			if(s[i] == 'B')	pos |= (1 << (9 - i));
		for(int i = 7; i < 10; ++i)
			if(s[i] == 'R')	pos |= (1 << (9 - i));
		highest = pos > highest ? pos : highest;
		seatedSeats[pos] = 1;
	}
	f.close();
	int id = 0;
	while(seatedSeats[id] == 0)
		++id;
	while(seatedSeats[id] == 1)
		++id;

	return { highest, id };
}


int main(int argc, char** argv)
{
	ReturnValues seat = readValues("data.txt");
	if(seat.highest == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	printf("Highest seat: %i\n", seat.highest);
	printf("Free seat: %i\n", seat.id);

	return 0;
}