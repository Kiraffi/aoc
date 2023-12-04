
#include <algorithm>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>

struct Asteroid
{
	int xpos;
	int ypos;
};

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


int checkInRange(int xp, int yp, int width, int height)
{
	return !(xp < 0 || xp >= width || yp < 0 || yp >= height);
}

void markAndCount(std::vector<std::string> &map, int xp, int yp, int xpos, int ypos,
	std::vector<Asteroid> &asteroids)
{
	int width = map[0].size();
	int height = map.size();

	int xpp = xp - xpos;
	int ypp = yp - ypos;

	bool found = false;
	while(checkInRange(xp, yp, width, height))
	{
		if(map[yp][xp] == '#' && !found)
		{
			found = true;
			asteroids.push_back(Asteroid{xp, yp});
			//printf("asteroid: %i, %i\n", xp, yp);
		}
		map[yp][xp] = '.';
		xp += xpp;
		yp += ypp;
	}

	return;
}

// Take local copy of map
std::vector<Asteroid> getHits(std::vector<std::string> map, int xpos, int ypos)
{
	std::vector<Asteroid> visibleAsteroids;
	int width = map[0].size();
	int height = map.size();
	int maxsize = width > height ? width : height;
	for(int k = 1; k < maxsize; ++k)
	{
		int xp = xpos - k;
		int yp = ypos - k;
		for(; xp <= xpos + k; ++xp)
		{
			markAndCount(map, xp, yp, xpos, ypos, visibleAsteroids);
		}
		--xp;
		for(; yp <= ypos + k; ++yp)
		{
			markAndCount(map, xp, yp, xpos, ypos, visibleAsteroids);
		}
		--yp;
		for(; xp >= xpos - k; --xp)
		{
			markAndCount(map, xp, yp, xpos, ypos, visibleAsteroids);
		}
		++xp;
		for(; yp >= ypos - k; --yp)
		{
			markAndCount(map, xp, yp, xpos, ypos, visibleAsteroids);
		}
		++yp;
	}

	//printf("Sees: %i, from: %i, %i\n", visibleAsteroids, xpos, ypos);
	return visibleAsteroids;
}

int main(int argc, char** argv)
{
	std::vector<std::string> map;
	if(!readValues("data.txt", map))
	{
		printf("Failed to load map!");
		return 0;
	};
	int width = map[0].size();
	int height = map.size(); 
	int maxSize = width < height ? height : width ;
	
	int maxAsteroids = 0;
	int bestX = 0;
	int bestY = 0;
	for(int jj = 0; jj < height; ++jj)
	{
		for(int ii = 0; ii < width; ++ii)
		{
			if(map[jj][ii] != '#')
				continue;
			std::vector<Asteroid> hits = getHits(map, ii, jj);
			if(hits.size() > maxAsteroids)
			{
				maxAsteroids = hits.size();
				bestX = ii;
				bestY = jj;
			}
			
		}
	}

	map[bestY][bestX] = 'X';
	// part 2
	{
		int destroyed = 0;
		int visibleAsteroids = maxAsteroids;
		// degrees
		float angle = -0.0001f;
		float newAngle = 0.0f;
		int destroyIndex = 0;
		std::vector<Asteroid> hits;
		Asteroid lastDestroyed = {};

		while(visibleAsteroids > 0 && destroyed < 200)
		{
			hits.clear();
			float closestAngle = 100000000000000.0f;
			hits = getHits(map, bestX, bestY);
			visibleAsteroids = hits.size();

			for(int i = 0; i < hits.size(); ++i)
			{
				const Asteroid &asteroid = hits[i];
				// y increases down
				int diffY = -(asteroid.ypos - bestY);
				int diffX = asteroid.xpos - bestX;
				float localAngle = (atan2(diffY, diffX)) * 180.0f / M_PI;
				// reversing the angle to clockwise, and starting from top.
				localAngle = 90.0f - localAngle;
				
				float angleDiff = localAngle - angle;
				while(angleDiff <= 0.0f)
					angleDiff += 360.0f;
				if(angleDiff < closestAngle)
				{
					newAngle = localAngle;
					closestAngle = angleDiff;
					destroyIndex = i;
				}
			}
			if(hits.size() > 0)
			{
				const Asteroid &asteroid = hits[destroyIndex];
				map[asteroid.ypos][asteroid.xpos] = '0' + destroyed % 10;
				++destroyed;
				lastDestroyed = asteroid;
				if(destroyed % 10 == 0)
				{
					printf("\n");
					for(int s = 0; s < map.size(); ++s)
						printf("%s\n", map[s].c_str());
					printf("\n");
					for(int jj = 0; jj < map.size(); ++jj)
						for(int ii = 0; ii < map[jj].size(); ++ii)
							if(!(map[jj][ii] == '#' || map[jj][ii] == 'X'))
								map[jj][ii] = '.';
				}
				angle = newAngle;
			}
		}
		printf("\n");
		for(int s = 0; s < map.size(); ++s)
			printf("%s\n", map[s].c_str());
		printf("\n");

		printf("Number of destroyed: %i. Last destroyed asteroid at: %i, %i\n", destroyed, 
			lastDestroyed.xpos, lastDestroyed.ypos);

	}

	printf("Part a: Can see %i asteroids from: %i, %i\n", maxAsteroids, bestX, bestY);


	return 0;
}