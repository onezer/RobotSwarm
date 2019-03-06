#pragma once
#include <atomic>
#include "MapGenerator.h"

class Map
{
	Map();
	std::atomic_int* MapArray;
	static Map* s_instance;
	int* size;
	int dimensions;
	

public:
	enum nodeType {Free, Obstacle, Robot};
	enum direction{North, South, West, East, Up, Down, NorthWest, NorthEast, SouthWest, SouthEast};
	enum mapType { twoD, threeD, hex };

	static Map* Instance();
	void SetMap(std::atomic_int* MapArray, mapType maptype, int* size);
	int getDimensions() const;
	int getType() const;
	const int* getSize() const;

	int Move(int* position, direction direction);
	int Look(int* position, direction direction);
	int PlaceRobot(int* position);
	int RemoveRobot(int* position);

	~Map();

private:
	mapType maptype;
	bool ValidPos(int* position) const;
	int getNode(int* position) const;
	void setNode(int* position, nodeType type);
	int* Transform(int* position, direction direction) const;
	void CopyPos(int* source, int* target) const;
	bool ValidDir(direction direction) const;
};

