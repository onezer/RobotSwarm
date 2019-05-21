#include "MapGenerator.h"
#include<string>
#include<time.h>
#include<list>
#include<queue>
#include<math.h>

MapGenerator* MapGenerator::s_instance;

MapGenerator::MapGenerator()
{
	*size = { 0 };

	dimensions = 0;
}




std::shared_ptr<std::list<Coord>> MapGenerator::GetRegionTiles(int startX, int startY)
{
	std::shared_ptr<std::list<Coord>> tiles = std::make_shared<std::list<Coord>>();

	int** mapFlags = new int*[size[0]];
	for (int i = 0; i < size[0]; ++i)
		mapFlags[i] = new int[size[1]];

	for (int x = 0; x < size[0]; x++) {
		for (int y = 0; y < size[1]; y++) {
			mapFlags[x][y] = 0;
		}
	}

	int tileType = ((Map::NodeObj**)mapArray)[startX][startY].type;

	std::queue<Coord>* queue = new std::queue<Coord>();
	Coord coordtmp(startX, startY);
	queue->push(coordtmp);
	mapFlags[startX][startY] = 1;

	while (queue->empty() == false) {
		Coord tile = queue->front();
		queue->pop();
		tiles->push_back(tile);

		for (int x = tile.tileX - 1; x <= tile.tileX + 1; x++) {
			for (int y = tile.tileY - 1; y <= tile.tileY + 1; y++) {
				if (IsInMapRange(x, y) && (y == tile.tileY || x == tile.tileX)) {
					if (mapFlags[x][y] == 0 && ((Map::NodeObj**)mapArray)[x][y].type == tileType) {
						mapFlags[x][y] = 1;
						Coord coordtmp(x,y);
						queue->push(coordtmp);
					}
				}
			}
		}
	}

	delete queue;

	for (int i = 0; i < this->size[0]; ++i) {
		delete[]mapFlags[i];
	}
	delete[]mapFlags;

	return tiles;
}

bool MapGenerator::IsInMapRange(int x, int y)
{
	return x >= 0 && x < size[0] && y >= 0 && y < size[1];
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

std::shared_ptr<std::list<std::shared_ptr<std::list<Coord>>>> MapGenerator::GetRegions(int tileType)
{
	std::shared_ptr<std::list<std::shared_ptr<std::list<Coord>>>> regions = std::make_shared<std::list<std::shared_ptr<std::list<Coord>>>>();

	int** mapFlags = new int*[size[0]];
	for (int i = 0; i < size[0]; ++i)
		mapFlags[i] = new int[size[1]];

	int width = size[0];
	int height = size[1];

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			mapFlags[x][y] = 0;
		}
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (mapFlags[x][y] == 0 && ((Map::NodeObj**)mapArray)[x][y].type == tileType) {
				std::shared_ptr<std::list<Coord>> newRegion = GetRegionTiles(x, y);
				regions->push_back(newRegion);

				for each(Coord tile in *newRegion) {
					mapFlags[tile.tileX][tile.tileY] = 1;
				}
			}
		}
	}

	for (int i = 0; i < this->size[0]; ++i) {
		delete[]mapFlags[i];
	}
	delete[]mapFlags;

	return regions;
}

void MapGenerator::ProcessMap()
{
	std::shared_ptr<std::list<std::shared_ptr<std::list<Coord>>>> wallRegions = GetRegions(1);
	size_t wallThresholdSize = 30;

	for each(std::shared_ptr<std::list<Coord>> wallRegion in *wallRegions) {

		if (wallRegion->size() < wallThresholdSize) {
			for each(Coord tile in *wallRegion) {
				((Map::NodeObj**)mapArray)[tile.tileX][tile.tileY].type = Map::nodeType::Free;
			}
		}
	}

	std::shared_ptr<std::list<std::shared_ptr<std::list<Coord>>>> roomRegions = GetRegions(0);
	size_t roomThresholdSize = 20;

	std::shared_ptr<std::list<Coord>> biggestRoom;
	size_t biggestSize = 0;

	for each(std::shared_ptr<std::list<Coord>> roomRegion in *roomRegions) {
		if (roomRegion->size() < roomThresholdSize) {
			for each(Coord tile in *roomRegion) {
				((Map::NodeObj**)mapArray)[tile.tileX][tile.tileY].type = Map::nodeType::Obstacle;
			}
			
		}
		else if (roomRegion->size() > biggestSize) {
			biggestSize = roomRegion->size();
			biggestRoom = roomRegion;
		}
	}

	if (biggestRoom->empty() == false) {
		startCoord = biggestRoom->back();
	}

	//((Map::NodeObj**)mapArray)[startCoord.tileX][startCoord.tileY].type = Map::nodeType::Nest;

}

MapGenerator * MapGenerator::Instance()
{
	if (!s_instance)
		s_instance = new MapGenerator;
	return s_instance;
}

Coord MapGenerator::GetStartCoord()
{
	return startCoord;
}

void MapGenerator::RandomFillMap() {
	int ratio = 60;

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

	for (int i = 0; i < 3; i++) {
		SmoothMap();
	}

	ProcessMap();

	Map::Instance()->SetMap(mapArray, type, size);
	mapArray = nullptr;
}

MapGenerator::~MapGenerator()
{
}




