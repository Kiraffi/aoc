
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

int64_t parseNumber(const char **ptr, bool gotoNext = true)
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
	if(*p != '\0' && gotoNext)
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

struct Tile
{
	std::vector<std::string> map;
	int neighbours[4] = {};
	int neighbourCount = 0;
	int tileId = 0;
};




bool checkMatch(const Tile& a, const Tile &b, int sideA, int sideB)
{
	int tileWidth = a.map[0].size();
	int tileHeight = a.map.size();
	assert(tileWidth == tileHeight);
	int tileSize = tileWidth;



	{
		for(int j = 0; j < tileSize; ++j)
		{
			int k1 = 0;
			int l1 = 0;
			switch(sideA)
			{
				case 0:
					k1 = 0;
					l1 = j;
					break;
				case 1:
					k1 = j;
					l1 = tileSize - 1;
					break;
				case 2:
					k1 = tileSize - 1;
					l1 = tileSize - j - 1;
					break;
				case 3:
					k1 = tileSize - j - 1;
					l1 = 0;
					break;
				case 4:
					k1 = 0;
					l1 = tileSize - j - 1;
					break;
				case 5:
					k1 = tileSize - j - 1;
					l1 = tileSize - 1;
					break;
				case 6:
					k1 = tileSize - 1;
					l1 = j;
					break;
				case 7:
					k1 = j;
					l1 = 0;
					break;
			}

			int k2 = 0;
			int l2 = 0;
			switch(sideB)
			{
				case 0:
					k2 = 0;
					l2 = j;
					break;
				case 1:
					k2 = j;
					l2 = tileSize - 1;
					break;
				case 2:
					k2 = tileSize - 1;
					l2 = tileSize - j - 1;
					break;
				case 3:
					k2 = tileSize - j - 1;
					l2 = 0;
					break;
				case 4:
					k2 = 0;
					l2 = tileSize - j - 1;
					break;
				case 5:
					k2 = tileSize - j - 1;
					l2 = tileSize - 1;
					break;
				case 6:
					k2 = tileSize - 1;
					l2 = j;
					break;
				case 7:
					k2 = j;
					l2 = 0;
					break;
			}

			if(a.map[k1][l1] != b.map[k2][l2])
			{
				return false;
			}
		}
	}
	return true;
}

void rotateForward(Tile &tile)
{
	int mapSize = tile.map.size();
	std::vector<std::string> newMap = tile.map;
	//assert(rotateTowards < 4);

	for(int j = 0; j < mapSize; ++j)
	{
		for(int i = 0; i < mapSize; ++i)
		{
			newMap[j][i] = tile.map[mapSize - i - 1][j];
		}
	}
	tile.map = newMap;
}

void rotateMirror(Tile &tile)
{
	int mapSize = tile.map.size();
	std::vector<std::string> newMap = tile.map;

	for(int j = 0; j < mapSize; ++j)
	{
		for(int i = 0; i < mapSize; ++i)
		{
			newMap[j][i] = tile.map[j][mapSize - i - 1];
		}
	}
	tile.map = newMap;
}

void rotateUntilMatch(const Tile &prevTile, Tile &rotateTile, int prevTileRot, int rotTileRot)
{
	int k = 0;
	while(true)
	{
		if((k % 4) == 0) rotateMirror(rotateTile);
		if(checkMatch(prevTile, rotateTile, prevTileRot, rotTileRot))
		{
			return;
		}
		else
		{
			rotateForward(rotateTile);
		}
		
		++k;
	}
}

void rotateUntilMatch(const Tile &prevTileLeft, const Tile &prevTileTop, Tile &rotateTile)
{
	int k = 0;
	while(true)
	{
		if((k % 4) == 0) rotateMirror(rotateTile);
		if((checkMatch(prevTileLeft, rotateTile, 1, 7) &&
			checkMatch(prevTileTop, rotateTile, 2, 4)))
		{
			return;
		}
		else
		{
			rotateForward(rotateTile);
		}
		
		++k;
	}
}

int main(int argc, char** argv)
{
	std::vector<std::string> values;

	if(readValues("data.txt", values) == -1)
	{
		printf("Failed to load map!");
		return 0;
	};

	std::vector<Tile> tiles;
	std::unordered_map<int, int> hashTiles;
	{
		Tile tile;
		for(const auto &s : values)
		{
			if(s.compare(0, 5, "Tile ") == 0)
			{
				const char *ptr = &s[5];
				tile.tileId = parseNumber(&ptr);
				tile.map.clear();
			}
			else if(s.length() == 0)
			{
				tiles.push_back(tile);
				tile.map.clear();
			}
			else
			{
				hashTiles.insert({tile.tileId, tiles.size()});
				tile.map.push_back(s);
			}
		}
		if(tiles[tiles.size() - 1].tileId != tile.tileId)
		{
			hashTiles.insert({tile.tileId, tiles.size()});
			tiles.push_back(tile);
		}
	}

	int tileWidth = tiles[0].map[0].size();
	int tileHeight = tiles[0].map.size();


	int64_t multips = 1;
	int cornerTileNumber = 0;
	for(int j = 0; j < tiles.size(); ++j)
	{
		Tile &compTile = tiles[j];
		//printf("\n\nTile: %i\n", compTile.tileId);
		for(int i = 0; i < tiles.size(); ++i)
		{
			if(i == j)
				continue;
			bool match = true;
			Tile &tile = tiles[i];

			for(int k = 0; k < 8; ++k)
			{
				for(int l = 0; l < 4; ++l)
				{
					if(checkMatch(tile, compTile, k, l))
					{
						assert(compTile.neighbourCount < 4);
						compTile.neighbours[compTile.neighbourCount] = tile.tileId;
						++compTile.neighbourCount;
					}
				}
			}	
		}
		assert(compTile.neighbourCount >= 2 && compTile.neighbourCount <= 4);
		if(compTile.neighbourCount == 2)
		{
			multips *= int64_t(compTile.tileId);
			//if(cornerTileNumber != 0)
				cornerTileNumber = compTile.tileId;
		}
	}
	printf("Part a: corner multiply: %" PRIi64 "\n", multips);

	std::vector<std::vector<int>> tileMap;
	Tile finalMapTile;

	{
		tileMap.push_back(std::vector<int>());
		Tile &tile = tiles[hashTiles[cornerTileNumber]];
		assert(tile.neighbourCount == 2);
		Tile &tile2 = tiles[hashTiles[tile.neighbours[0]]];
		Tile &tile3 = tiles[hashTiles[tile.neighbours[1]]];
	
		tileMap[0].push_back(tile.tileId);
		tileMap[0].push_back(tile2.tileId);

		// Rotate first corner piece into correct position
		{
			bool found1 = false;
			bool found2 = false;
			int k = 0;
			while(!(found1 && found2))
			{
				found1 = false;
				found2 = false;
				if((k % 4) == 0) rotateMirror(tile);
				for(int l = 0; l < 8; ++l)
				{
					found1 |= checkMatch(tile, tile2, 1, l);
					found2 |= checkMatch(tile, tile3, 2, l);
				}
				if(!found1 || !found2)
					rotateForward(tile);
				++k;
			}
		}
		rotateUntilMatch(tile, tile2, 1, 7);
		rotateUntilMatch(tile, tile3, 2, 4);

		// Set columns first row
		Tile *lastTile = &tile2;
		while(lastTile->neighbourCount == 3)
		{
			int cols = tileMap[0].size();
			Tile *newTile = nullptr;
			for(int i = 0; i < lastTile->neighbourCount; ++i)
			{
				newTile = &tiles[hashTiles[lastTile->neighbours[i]]];
				if(newTile->neighbourCount < 4 && 
					newTile->tileId != tiles[hashTiles[tileMap[0][cols - 2]]].tileId)
				{
					rotateUntilMatch(*lastTile, *newTile, 1, 7);
					lastTile = newTile;
					++cols;
					tileMap[0].push_back(newTile->tileId);
					break;
				}
			}
		}

		int tileMapWidth = tileMap[0].size();
		tileMap.push_back(std::vector<int>(tileMapWidth, 0));

		tileMap[1][0] = tile3.tileId;

		// do rows first col
		lastTile = &tile3;
		while(lastTile->neighbourCount == 3)
		{
			int rows = tileMap.size();
			Tile *newTile = nullptr;
			for(int i = 0; i < 3; ++i)
			{
				newTile = &tiles[hashTiles[lastTile->neighbours[i]]];
				if(newTile->neighbourCount < 4 && 
					newTile->tileId != tiles[hashTiles[tileMap[rows - 1][0]]].tileId)
				{
					newTile = &tiles[hashTiles[lastTile->neighbours[i]]];
					if(newTile->neighbourCount < 4 && 
						newTile->tileId != tiles[hashTiles[tileMap[rows - 2][0]]].tileId)
					{
						rotateUntilMatch(*lastTile, *newTile, 2, 4);
						lastTile = newTile;
						tileMap.push_back(std::vector<int>(tileMapWidth, 0));
						tileMap[rows][0] = (newTile->tileId);
						++rows;
						break;
					}
				}
			}
		}
		int tileMapHeight = tileMap.size();

		// Set the rest
		for(int j = 1; j < tileMapHeight; ++j)
		{
			for(int i = 1; i < tileMapWidth; ++i)
			{
				const Tile &topTile = tiles[hashTiles[tileMap[j - 1][i]]];
				const Tile &leftTile = tiles[hashTiles[tileMap[j][i - 1]]];
				const Tile &topLeftTile = tiles[hashTiles[tileMap[j - 1][i - 1]]];

				int tileIndex = 0;
				for(int i2 = 0; i2 < topTile.neighbourCount; ++i2)
				{
					if(topTile.neighbours[i2] == topLeftTile.tileId)
						continue;
					tileIndex =  topTile.neighbours[i2];

					for(int j2 = 0; j2 < leftTile.neighbourCount; ++j2)
					{
						if(tileIndex == leftTile.neighbours[j2])
						{
							goto foundIndex;
						}
					}
				}
				assert(0);
				foundIndex:
				Tile &newTile = tiles[hashTiles[tileIndex]];
				bool f1 = false;
				bool f2 = false;
				for(int mm = 0; mm < newTile.neighbourCount; ++mm)
				{
					f1 |= (newTile.neighbours[mm] == topTile.tileId);
					f2 |= (newTile.neighbours[mm] == leftTile.tileId);
				}
				assert(f1 && f2);
				rotateUntilMatch(leftTile, topTile, newTile);
				assert(checkMatch(leftTile, newTile, 1, 7));
				assert(checkMatch(topTile, newTile, 2, 4));

				tileMap[j][i] = tileIndex;

			}
		}

		std::vector<std::string> newMap((tileHeight - 2) * tileMapHeight);

		for(int k = 0; k < tileMap.size(); ++k)
		{
			for(int j = 0; j < tileHeight; ++j)
			{
				for(int i = 0; i < tileMapWidth; ++i)
				{
					const Tile &tile = tiles[hashTiles[tileMap[k][i]]];
					if(j > 0 && j < tileHeight - 1)
						newMap[(j - 1) + k * (tileHeight - 2)] += tile.map[j].substr(1, tileWidth - 2);
				}
			}
		}
		finalMapTile.map = newMap;
	}

	{
		std::vector<std::string> monster;
		monster.push_back("                  # ");
		monster.push_back("#    ##    ##    ###");
		monster.push_back(" #  #  #  #  #  #   ");
		int monsterWidth = monster[0].size();
		int monsterHeight = monster.size();

		int monsterHashCount = 0;
		for(int l = 0; l < monsterHeight; ++l)
		{
			for(int k = 0; k < monsterWidth; ++k)
			{
				monsterHashCount += monster[l][k] == '#';
			}
		}

		for(int rot = 0; rot < 8; ++rot)
		{
			if(rot == 4)
				rotateMirror(finalMapTile);
			int monsterCount = 0;
			for(int j = 0; j < finalMapTile.map.size() - monsterHeight; ++j)
			{
				for(int i = 0; i < finalMapTile.map[0].size() - monsterWidth; ++i)
				{
					int monsterHitCount = 0;
					for(int l = 0; l < monsterHeight; ++l)
					{
						for(int k = 0; k < monsterWidth; ++k)
						{
							char mChar = monster[l][k];
							char fmChar = finalMapTile.map[j + l][i + k];
							if(mChar == '#' && (fmChar == '#' || fmChar == 'O'))
							{
								++monsterHitCount;
							}
						}
					}
					if(monsterHitCount == monsterHashCount)
					{
						++monsterCount;
						for(int l = 0; l < monsterHeight; ++l)
						{
							for(int k = 0; k < monsterWidth; ++k)
							{
								char mChar = monster[l][k];
								char fmChar = finalMapTile.map[j + l][i + k];
								if(mChar == '#' && (fmChar == '#' || fmChar == 'O'))
								{
									finalMapTile.map[j + l][i + k] = 'O';
								}
							}
						}
					}
				}
			}
			if(monsterCount)
				break;
			rotateForward(finalMapTile);
		}

		int hashes = 0;


		for(const auto &s : finalMapTile.map)
		{
			for(const auto &c : s)
				hashes += c == '#';
		}
		printf("hashes left: %i", hashes);
	}


	return 0;
}