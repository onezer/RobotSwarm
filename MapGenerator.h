#pragma once
#include"Map.h"

//Singleton class that procedurally generates map with given parameters
class MapGenerator
{
	MapGenerator();

	static MapGenerator* s_instance;
	void* mapArray;
	void RandomFillMap();
	void SmoothMap();
	int GetSurroundingWallCount(int gridX, int gridY);
	Map::mapType type;
	int size[3];
	int dimensions;

	void Create();

public:
	static MapGenerator* Instance();
	
	void GenerateMap(Map::mapType type, int* size, bool randomSeed, unsigned int seed=0);
	~MapGenerator();
};