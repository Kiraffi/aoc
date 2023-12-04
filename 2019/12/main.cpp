
#include <algorithm>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

struct IVec3
{
	int x = 0;
	int y = 0;
	int z = 0;
};

struct Moon
{
	IVec3 pos;
	IVec3 vel;
};


bool readValues(const char *filename, std::vector<Moon> &outMoons)
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
		Moon moon;
		int i = 0;
		int *ptr = &moon.pos.x;
		while(i < s.length())
		{
			while(s[i] != '=' && i < s.length())
				++i;
			if(i >= s.length() )
				break;
			++i;
			*ptr = atoi(&(s.c_str()[i]));
			++ptr;
		}

		outMoons.push_back(moon);
	}
	f.close();
	
	return true;
}

int comp(int a, int b)
{
	if(a < b)
		return 1;
	else if(a > b)
		return -1;
	return 0;
}

int abss(int a)
{
	return a > 0 ? a : -a;
}

int64_t maxx(int64_t a, int64_t b)
{
	return a > b ? a : b;
}

void updateMoonsAxis(std::vector<Moon> &moons, int axis)
{
	for(Moon &moon : moons)
	{
		for(const Moon &moon2 : moons)
		{
			if(&moon == &moon2)
				continue;
			
			(&moon.vel.x)[axis] += comp((&moon.pos.x)[axis], (&moon2.pos.x)[axis]);
		}
	}
	bool allZero = true;
	for(Moon &moon : moons)
	{
		(&moon.pos.x)[axis] += (&moon.vel.x)[axis];
	}
}

int main(int argc, char** argv)
{
	std::vector<Moon> moons;
	if(!readValues("data.txt", moons))
	{
		printf("Failed to load map!");
		return 0;
	};

	std::vector<Moon> previousMoons = moons;

	{
		// Part a
		int64_t energy = 0;
		for(int loop = 0; loop < 1000; ++loop)
		{
			for(int i = 0; i < 3; ++i)
				updateMoonsAxis(moons, i);
		}
		for(const Moon &moon : moons)
		{
			energy += int64_t(abss(moon.pos.x) + abss(moon.pos.y) + abss(moon.pos.z)) * 
				int64_t(abss(moon.vel.x) + abss(moon.vel.y) + abss(moon.vel.z));
		}

		printf("Energy: %" PRId64 "\n", energy);

	}

	// Part b
	{
		int cycles[3] = {};

		// Find a cycle for each axis separately to find
		// when the position and velocity is same as in the beginning
		for(int axis = 0; axis < 3; ++axis)
		{
			moons = previousMoons;
			int64_t loop = 1;
			int found = 0;
			while(found < 1)
			{
				updateMoonsAxis(moons, axis);

				bool sameVel = true;
				for(const Moon &moon : moons)
				{
					if((&moon.vel.x)[axis])
						sameVel = false;
				}

				bool samePos = sameVel;
				if(sameVel)
				{
					for(int i = 0; i < moons.size(); ++i)
					{
						const Moon &moon = moons[i];
						const Moon &moon2 = previousMoons[i];
						if(((&moon.pos.x)[axis]) != ((&moon2.pos.x)[axis]))
							samePos = false;
					}
					if(samePos)
					{
						cycles[axis] = loop;
						++found;
					}
				}
				++loop;
			}
		}

		// find primes up to max
		int64_t number = maxx(
			maxx(cycles[0], cycles[1]), cycles[2]);

		std::vector<int64_t> primes;
		for(int64_t i = 2; i < number; ++i)
		{
			bool wasDivised = false;
			for(int64_t prime : primes)
			{
				if((i % prime) == 0)
				{
					wasDivised = true;
					break;
				}
			}
			if(!wasDivised)
				primes.push_back(i);
		}

		// calculate least common multiple, to find
		// a point where the position is same as start
		// for all axes.
		int64_t multi = 1;
		for(int64_t prime : primes)
		{
			int64_t maxLoop = 1;
			for(int i = 0; i < 3; ++i)
			{
				int64_t l = cycles[i];
				int64_t localLoop = 1;
				while(l % prime == 0 && l > prime)
				{
					localLoop *= prime;
					l /= prime;
				}
				maxLoop = maxx(maxLoop, localLoop);
			}
			multi *= maxLoop;
		}

		printf("Repetition at: %" PRId64 "\n", multi);
	}
	return 0;
}