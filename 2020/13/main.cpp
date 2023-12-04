
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

int readValues(const char *filename, std::vector<int64_t> &outNumbers)
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
		const char *ptr = &s[0];

		while(*ptr)
		{
			outNumbers.push_back(parseNumber(&ptr));
			while(*ptr && *ptr != ',') ++ptr;
			if(*ptr == ',') ++ptr;
		}
	}
	f.close();

	return 0;
}

void partA(const std::vector<int64_t> &numbers)
{
	int64_t arriveTime = numbers[0];

	int64_t shortestTime = (int64_t(1) << int64_t(60));
	int64_t shortestTimeBus = 0;

	for(int i = 1; i < numbers.size(); ++i)
	{
		
		int64_t interval = numbers[i];
		if(interval > 0)
		{
			int64_t dur = interval - (arriveTime % interval);
			if(dur < shortestTime)
			{
				shortestTime = dur;
				shortestTimeBus = dur * interval;
			}
		}
	}
	printf("Shortest time: %" PRIi64 ", shortest bus time: %" PRIi64 "\n", 
		shortestTime, shortestTimeBus);
}


void partB(const std::vector<int64_t> &numbers)
{
	struct IntervalOffset
	{
		int64_t interval;
		int64_t offset;
	};
	std::vector<IntervalOffset> intoffs;
	// ordering the intervals from highest to lowest.
	for(int i = 1; i < numbers.size(); ++i)
	{
		int64_t n = numbers[i];
		if(n > 0)
		{
			int64_t insertPos = intoffs.size();
			for(int j = 0; j < intoffs.size(); ++j)
			{
				if(n > intoffs[j].interval)
				{
					insertPos = j;
					break;
				}
			}
			intoffs.insert(intoffs.begin() + insertPos, IntervalOffset{n, i - 1});
		}
	}
	int64_t interval = intoffs[0].interval;
	int64_t startTime = interval - intoffs[0].offset;

	for(const IntervalOffset &intoff : intoffs)
	{
		int64_t offs = (startTime + intoff.offset) % intoff.interval;
		// If the interval isnt correct...
		if(offs != 0)
		{
			// find a next suitable start time by adding interval times n
			while(offs != 0)
			{
				startTime += interval;
				offs = (offs + interval) % intoff.interval;
			}
			offs += interval;
			int64_t m = 1;
			// In order to calculate future intervals, we calculate minimal
			// increment to interval so that it can always match this number.
			while(offs != 0)
			{
				m += 1;
				offs = (offs + interval) % intoff.interval;
			}
			interval = m * interval;
		}
	}

	printf("Combined time: %" PRIi64 "\n", startTime);
}


int main(int argc, char** argv)
{
	std::vector<int64_t> numbers;
	if(readValues("data.txt", numbers) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	partA(numbers);
	partB(numbers);
	return 0;
}