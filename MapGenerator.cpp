#include "MapGenerator.h"
#include<string>
#include<time.h>

MapGenerator* MapGenerator::s_instance;

MapGenerator::MapGenerator()
{
	*size = { 0 };

	dimensions = 0;
}


void MapGenerator::Create()
{
	if (dimensions == 2) {
		mapArray = new Map::NodeObj*[size[0]];
		for (int i = 0; i < size[0]; ++i) {
			((Map::NodeObj**)mapArray)[i] = new Map::NodeObj[size[1]];
		}
	}
	else if (dimensions == 3) {
		mapArray = new Map::NodeObj**[size[0]];
		for (int i = 0; i < size[0]; ++i) {
			((Map::NodeObj***)mapArray)[i] = new Map::NodeObj*[size[1]];
			for (int j = 0; j < size[1]; ++j) {
				((Map::NodeObj***)mapArray)[i][j] = new Map::NodeObj[size[2]];
			}
		}
	}
}

MapGenerator * MapGenerator::Instance()
{
	if (!s_instance)
		s_instance = new MapGenerator;
	return s_instance;
}

void MapGenerator::RandomFillMap() {
	int ratio = 55;

	if(dimensions == 2){
		for (int y = 0; y < size[1]; ++y) {
			for (int x = 0; x < size[0]; ++x) {
				if (x == 0 || x == size[0] - 1 || y == 0 || y == size[1]) {
					((Map::NodeObj**)mapArray)[x][y].type = Map::nodeType::Obstacle;
				}
				else {
					if (std::rand() % 100 < ratio) {
						((Map::NodeObj**)mapArray)[x][y].type = Map::nodeType::Free;
					}
					else {
						((Map::NodeObj**)mapArray)[x][y].type = Map::nodeType::Obstacle;
					}
				}
			}
		}
	}
	
}

void MapGenerator::SmoothMap()
{
	if (dimensions == 2) {
		for (int x = 0; x < size[0]; x++) {
			for (int y = 0; y < size[1]; y++) {
				int neighbourWallTiles = GetSurroundingWallCount(x, y);

				if (neighbourWallTiles > 4)
					((Map::NodeObj**)mapArray)[x][y].type = Map::nodeType::Obstacle;
				else if (neighbourWallTiles < 4)
					((Map::NodeObj**)mapArray)[x][y].type = Map::nodeType::Free;

			}
		}
	}
}

int MapGenerator::GetSurroundingWallCount(int gridX, int gridY)
{
	int wallCount = 0;
	if (dimensions == 2) {
		for (int neighbourX = gridX - 1; neighbourX <= gridX + 1; neighbourX++) {
			for (int neighbourY = gridY - 1; neighbourY <= gridY + 1; neighbourY++) {
				if (neighbourX >= 0 && neighbourX < size[0] && neighbourY >= 0 && neighbourY < size[1]) {
					if (neighbourX != gridX || neighbourY != gridY) {
						wallCount += ((Map::NodeObj**)mapArray)[neighbourX][neighbourY].GetType();
					}
				}
				else {
					wallCount++;
				}
			}
		}
	}

	return wallCount;
}

void MapGenerator::GenerateMap(Map::mapType type, int * size, bool randomSeed, unsigned int seed)
{
	if (!randomSeed) {
		std::srand(seed);
	}
	else {
		std::srand(time(NULL));
	}

	this->type = type;

	if (type == Map::mapType::twoD || type == Map::mapType::hex) {
		dimensions = 2;
	}
	else if (type == Map::mapType::threeD) {
		dimensions = 3;
	}

	std::memcpy(this->size, size, dimensions * sizeof(int));

	//If a mapArray is already allocated with the same size, then use that
	mapArray = Map::Instance()->Recycle(size);
	
	if (mapArray == nullptr) {
		Create();
	}

	RandomFillMap();

	for (int i = 0; i < 2; i++) {
		SmoothMap();
	}

	Map::Instance()->SetMap(mapArray, type, size);
	mapArray = nullptr;
}

MapGenerator::~MapGenerator()
{
}
