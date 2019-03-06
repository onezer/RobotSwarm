#include "Map.h"
#include<iostream>
#include<atomic>
#include<exception>



Map * Map::Instance()
{
	if (!s_instance)
		s_instance = new Map();
	return s_instance;
}

void Map::SetMap(std::atomic_int * MapArray, mapType maptype, int* size)
{
	this->MapArray = MapArray;
	this->size = size;
	this->maptype = maptype;
	switch (maptype) {
	case twoD: dimensions = 2; break;
	case threeD: dimensions = 3; break;
	case hex: dimensions = 3; break;
	default: throw new std::invalid_argument("SetMap: Wrong maptype!"); break;
	}
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
	if (!ValidDir) {
		throw new std::invalid_argument("Robot Move: invalid direction!");
	}

	if (!ValidPos(position)) {
		throw new std::invalid_argument("Robot Move: invalid robot position, it's out of range!");
	}

	if (getNode(position) != nodeType::Robot) {
		throw new std::invalid_argument{ "Robot Move: invalid robot position, it's not a robot's position!" };
	}

	int* newPos = Transform(position, direction);
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
		delete[] newPos;
		return 1;
	}
	m_Move.unlock();

	delete[] newPos;
	return 0;
}

int Map::Look(int * position, direction direction)
{
	if (!ValidDir) {
		throw new std::invalid_argument("Robot Look: invalid direction!");
	}

	if (!ValidPos(position)) {
		throw new std::invalid_argument("Robot Look: invalid robot position, it's out of range!");
	}

	if (getNode(position) != nodeType::Robot) {
		throw new std::invalid_argument{ "Robot Look: invalid robot position, it's not a robot's position!" };
	}

	int* newPos = Transform(position, direction);
	if (!newPos) {
		return nodeType::Obstacle;
	}

	int node = getNode(newPos);

	delete[] newPos;

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
		return MapArray[position[0], position[1]];
	}
	else {
		return MapArray[position[0], position[1], position[2]];
	}
}

void Map::setNode(int * position, nodeType type)
{
	if (dimensions == 2) {
		MapArray[position[0], position[1]] = type;
	}
	else {
		MapArray[position[0], position[1], position[2]] = type;
	}
}

int * Map::Transform(int * position, direction direction) const
{
	if (ValidPos(position)) {
		throw new std::invalid_argument("Transform: Invalid position, out of range!");
	}

	int* newPos = new int[dimensions];

	for (int i = 0; i < dimensions; ++i) {
		newPos[i] = position[i];
	}

	if (!ValidDir(direction)) {
		throw new std::invalid_argument("Transform: Invalid direction!");
	}

	switch (direction) {
	case North: newPos[1] += 1; break;
	case South: newPos[1] -= 1; break;
	case West: newPos[0] -= 1; break;
	case East: newPos[0] += 1; break;
	case Up: newPos[2] += 1; break;
	case Down: newPos[2] -= 1; break;
	case NorthWest: 
		newPos[1] += 1;
		newPos[0] -= 1;
		break;
	case NorthEast: newPos[1] += 1; break;
	case SouthWest: newPos[1] -= 1; break;
	case SouthEast:
		newPos[1] -= 1;
		newPos[0] += 1;
		break;
	}

	if (ValidPos(newPos)) {
		return newPos;
	}
	else {
		return nullptr;
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
