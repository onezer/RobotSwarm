#pragma once
#include"Map.h"
#include"Coord.h"
#include"Room.h"
#include<memory>

//Singleton class that procedurally generates map with given parameters
class MapGenerator
{
	MapGenerator();

	static MapGenerator* s_instance;
	void* mapArray;
	void RandomFillMap();
	void SmoothMap();
	int GetSurroundingWallCount(int gridX, int gridY);
	Coord startCoord;
	Map::mapType type;
	int size[3];
	int dimensions;
	std::shared_ptr<std::list<Coord>> GetRegionTiles(int startX, int startY);
	bool IsInMapRange(int x, int y);
	void Create();
	std::shared_ptr<std::list<std::shared_ptr<std::list<Coord>>>> GetRegions(int tileType);
	void ProcessMap();

public:
	static MapGenerator* Instance();

	Coord GetStartCoord();
	
	void GenerateMap(Map::mapType type, int* size, bool randomSeed, unsigned int seed=0);
	~MapGenerator();
};