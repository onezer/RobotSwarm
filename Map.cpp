#include "Map.h"

Map * Map::Instance()
{
	if (!s_instance)
		s_instance = new Map();
	return s_instance;
}

void Map::SetMap(void* MapArray, mapType maptype, int* size)
{
	this->MapArray = MapArray;
	this->maptype = maptype;

	switch (maptype) {
	case twoD: dimensions = 2; break;
	case threeD: dimensions = 3; break;
	case hex: dimensions = 2; break;
	default: throw new std::invalid_argument("SetMap: Wrong maptype!"); break;
	}

	int* mySize = new int[dimensions];
	CopyPos(size, mySize);
	this->size = mySize;
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
	if(!newPos){
		return 2; // movement out of range
	}

	m_Move.lock();
	if (getNode(newPos) == nodeType::Free) {
		setNode(newPos, nodeType::Robot);
		setNode(position, nodeType::Free);
		CopyPos(newPos, position);
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

	if (getNode(position) != nodeType::Robot) {
		throw new std::invalid_argument{ "Robot Look: invalid robot position, it's not a robot's position!" };
	}

	int newPos[3];

	Transform(position, direction, newPos);
	if (!newPos) {
		return nodeType::Obstacle;
	}

	int node = getNode(newPos);

	return node;
}

int Map::PlaceRobot(int * position)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("PlaceRobot: invalid robot position, it's out of range!");
	}

	if (getNode(position) != nodeType::Free) {
		return 1;
	}

	setNode(position, nodeType::Robot);

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

	setNode(position, nodeType::Free);

	return 0;
}

Map* Map::s_instance;

Map::Map()
{
}


Map::~Map()
{
	delete[] size;
	delete[] MapArray;
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

	CopyPos(position, newPosition);

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

void Map::CopyPos(int * source, int * target) const
{
	for (int i = 0; i < dimensions; ++i) {
		target[i] = source[i];
	}
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
