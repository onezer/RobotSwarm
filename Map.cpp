#include "Map.h"
#include<iostream>

Map* Map::s_instance;

Map * Map::Instance()
{
	if (!s_instance)
		s_instance = new Map();
	return s_instance;
}

void Map::SetMap(void* MapArray, mapType maptype, int* size)
{
	if (this->MapArray != nullptr) {
		if (dimensions == 2) {
			for (int i = 0; i < this->size[0]; ++i) {
				delete[] ((std::atomic_int**)this->MapArray)[i];
			}
			delete[](std::atomic_int**)this->MapArray;
		}
	}

	this->MapArray = MapArray;
	this->maptype = maptype;

	switch (maptype) {
	case twoD: dimensions = 2; break;
	case threeD: dimensions = 3; break;
	case hex: dimensions = 2; break;
	default: throw new std::invalid_argument("SetMap: Wrong maptype!"); break;
	}

	//CopyPos(size, this->size);
	std::memcpy(this->size, size, dimensions * sizeof(int));
}

int Map::getDimensions() const
{
	return dimensions;
}

int Map::getType() const
{
	return maptype;
}

const int* Map::getSize() const
{
	return size;
}

int Map::Move(int * position, direction direction)
{
	if (!ValidDir(direction)) {
		throw new std::invalid_argument("Robot Move: invalid direction!");
	}

	if (!ValidPos(position)) {
		throw new std::invalid_argument("Robot Move: invalid robot position, it's out of range!");
	}

	if (getNode(position) != nodeType::Robot) {
		throw new std::invalid_argument{ "Robot Move: invalid robot position, it's not a robot's position!" };
	}

	int newPos[3];

	Transform(position, direction, newPos);
	if(!ValidPos(newPos)){
		return 2; // movement out of range
	}

	m_Move.lock();
	if (getNode(newPos) == nodeType::Free) {
		setNode(newPos, nodeType::Robot);
		setNode(position, nodeType::Free);
		std::memcpy(position, newPos, dimensions*sizeof(int));
	}
	else {
		m_Move.unlock();
		return 1; //collision
	}
	m_Move.unlock();

	return 0;
}

int Map::Look(int * position, direction direction)
{
	if (!ValidDir(direction)) {
		throw new std::invalid_argument("Robot Look: invalid direction!");
	}

	if (!ValidPos(position)) {
		throw new std::invalid_argument("Robot Look: invalid robot position, it's out of range!");
	}

	try {
		if (getNode(position) != nodeType::Robot) {
			throw new std::invalid_argument{ "Robot Look: invalid robot position, it's not a robot's position!" };
		}
	}
	catch (std::invalid_argument) {
		throw;
	}

	int newPos[3];

	try {
		Transform(position, direction, newPos);
	}
	catch (std::invalid_argument) {
		throw;
	}

	if (!ValidPos(newPos)) {
		return nodeType::Obstacle;
	}

	int node;

	try {
		node = getNode(newPos);
	}
	catch (std::invalid_argument) {
		throw;
	}

	return node;
}

int Map::PlaceRobot(int * position)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("PlaceRobot: invalid robot position, it's out of range!");
	}

	try {
		if (getNode(position) != nodeType::Free) {
			return 1;
		}
	}
	catch (std::invalid_argument) {
		throw;
	}

	try {
		setNode(position, nodeType::Robot);
	}
	catch (std::invalid_argument) {
		throw;
	}
	return 0;
}

int Map::RemoveRobot(int * position)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("RemoveRobot: invalid robot position, it's out of range!");
	}

	if (getNode(position) != nodeType::Robot) {
		return 1;
	}

	try {
		setNode(position, nodeType::Free);
	}
	catch (std::invalid_argument) {
		throw;
	}

	return 0;
}

void Map::Clean()
{
	if (dimensions == 2) {
		for (int y = size[1] - 1; y >= 0; --y) {
			for (int x = 0; x < size[0]; ++x) {
				if (((std::atomic_int**)MapArray)[x][y] == nodeType::Robot) {
					((std::atomic_int**)MapArray)[x][y] = nodeType::Free;
				}
			}
		}
	}
	else if (dimensions == 3) {
		for (int y = size[1] - 1; y >= 0; --y) {
			for (int x = 0; x < size[0]; ++x) {
				for (int z = 0; z < size[2]; z++) {
					if (((std::atomic_int***)MapArray)[x][y][z] == nodeType::Robot) {
						((std::atomic_int***)MapArray)[x][y][z] = nodeType::Free;
					}
				}
				
			}
		}
	}
}

void * Map::Recycle(int * size)
{
	for (int i = 0; i < dimensions; ++i) {
		if (size[i] != this->size[i]) {
			return nullptr;
		}
	}

	return MapArray;
}

void Map::DisplayMap() const
{
	for (int y = size[1] - 1; y >= 0; --y) {
		for (int x = 0; x < size[0]; ++x) {
			switch (((std::atomic_int**)MapArray)[x][y]) {
			case 0: std::cout << "-"; break;
			case 1: std::cout << "X"; break;
			case 2: std::cout << "O"; break;
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;
}

Map::Map()
{
	*size = { 0 };

	MapArray = nullptr;
}


Map::~Map()
{
	if (this->MapArray != nullptr) {
		if (dimensions == 2) {
			for (int i = 0; i < this->size[0]; ++i) {
				delete[]((std::atomic_int**)this->MapArray)[i];
			}
			delete[](std::atomic_int**)this->MapArray;
		}
	}
}

int Map::getNode(int * position) const
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("GetNode: Invalid position!");
	}

	if (dimensions == 2) {
		return ((std::atomic_int**)MapArray)[position[0]][position[1]];
	}
	else {
		return ((std::atomic_int***)MapArray)[position[0]][position[1]][position[2]];
	}
}

void Map::setNode(int * position, nodeType type)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("SetNode: Invalid position!");
	}

	if (dimensions == 2) {
		((std::atomic_int**)MapArray)[position[0]][position[1]] = type;
	}
	else {
		((std::atomic_int***)MapArray)[position[0]][position[1]][position[2]] = type;
	}
}

void Map::Transform(int * position, direction direction, int* newPosition) const
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("Transform: Invalid position, out of range!");
	}

	if (!ValidDir(direction)) {
		throw new std::invalid_argument("Transform: Invalid direction!");
	}

	memcpy(newPosition, position, dimensions * sizeof(int));

	switch (direction) {
	case North: newPosition[1] += 1; break;
	case South: newPosition[1] -= 1; break;
	case West: newPosition[0] -= 1; break;
	case East: newPosition[0] += 1; break;
	case Up: newPosition[2] += 1; break;
	case Down: newPosition[2] -= 1; break;
	case NorthWest: 
		newPosition[1] += 1;
		newPosition[0] -= 1;
		break;
	case NorthEast: newPosition[1] += 1; break;
	case SouthWest: newPosition[1] -= 1; break;
	case SouthEast:
		newPosition[1] -= 1;
		newPosition[0] += 1;
		break;
	}
}

bool Map::ValidPos(int * position) const
{
	for (int i = 0; i < dimensions; ++i) {
		if (position[i] < 0) {
			return false;
		}

		if (position[i] >= size[i]) {
			return false;
		}
	}

	return true;
}


bool Map::ValidDir(direction direction) const {
	switch (maptype) {
	case twoD: 
		switch (direction) {
		case North: return true; break;
		case South: return true; break;
		case West: return true; break;
		case East: return true; break;
		default: return false; break;
		}
		break;
	case threeD:
		switch (direction) {
		case North: return true; break;
		case South: return true; break;
		case West: return true; break;
		case East: return true; break;
		case Up: return true; break;
		case Down: return true; break;
		default: return false; break;
		}
		break;
	case hex:
		switch (direction) {
		case SouthWest: return true; break;
		case SouthEast: return true; break;
		case West: return true; break;
		case East: return true; break;
		case NorthWest: return true; break;
		case NorthEast: return true; break;
		default: return false; break;
		}
		break;
	}
	
}
