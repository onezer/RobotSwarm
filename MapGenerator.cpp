#include "MapGenerator.h"

MapGenerator* MapGenerator::s_instance;

MapGenerator::MapGenerator()
{
}


MapGenerator * MapGenerator::Instance()
{
	if (!s_instance)
		s_instance = new MapGenerator;
	return s_instance;
}

int * MapGenerator::GenerateMap(Map::mapType type, int * size, bool randomSeed, std::string seed)
{
	return nullptr;
}

MapGenerator::~MapGenerator()
{
}
