#pragma once
#include"Map.h"
class MapGenerator
{
	MapGenerator();

	static MapGenerator* s_instance;

public:
	static MapGenerator* Instance();
	int* GenerateMap(Map::mapType type, int* size, bool randomSeed, std::string seed="");
	~MapGenerator();
};