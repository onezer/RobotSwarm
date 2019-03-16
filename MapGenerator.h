#pragma once
#include"Map.h"
class MapGenerator
{
	MapGenerator();

	static MapGenerator* s_instance;
	void* mapArray;
	void RandomFillMap(bool randomSeed, unsigned int seed);
	void SmoothMap();
	int GetSurroundingWallCount(int gridX, int gridY);
	Map::mapType type;
	int size[3];

public:
	static MapGenerator* Instance();
	
	void GenerateMap(Map::mapType type, int* size, bool randomSeed, unsigned int seed=0);
	~MapGenerator();
};