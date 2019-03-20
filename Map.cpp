#include "Map.h"
#include<iostream>
#include <cv.h>
#include <highgui.h>

Map* Map::s_instance;

Map * Map::Instance()
{
	if (!s_instance)
		s_instance = new Map();
	return s_instance;
}

void Map::SetMap(void* MapArray, mapType maptype, int* size)
{
	DeleteMap();

	this->MapArray = MapArray;
	this->maptype = maptype;

	switch (maptype) {
	case twoD: dimensions = 2; break;
	case threeD: dimensions = 3; break;
	case hex: dimensions = 2; break;
	default: throw new std::invalid_argument("SetMap: Wrong maptype!"); break;
	}

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

	if (getNode(position).type != nodeType::Robot) {
		throw new std::invalid_argument{ "Robot Move: invalid robot position, it's not a robot's position!" };
	}

	int newPos[3];

	Transform(position, direction, newPos);
	if(!ValidPos(newPos)){
		return 2; // movement out of range
	}

	m_Move.lock();
	try {
		if (getNode(newPos).type == nodeType::Free) {
			setNode(newPos, NodeObj(nodeType::Robot, getNode(position).id));
			setNode(position, NodeObj(nodeType::Free));
			std::memcpy(position, newPos, dimensions * sizeof(int));
		}
		else {
			m_Move.unlock();
			return 1; //collision
		}
		m_Move.unlock();
	}
	catch (std::exception& e) {
		std::cout << e.what() << ": Move\n";
		m_Move.unlock();
		throw;
	}

	return 0;
}

Map::NodeObj Map::Look(int * position, direction direction)
{
	if (!ValidDir(direction)) {
		throw new std::invalid_argument("Robot Look: invalid direction!");
	}

	if (!ValidPos(position)) {
		throw new std::invalid_argument("Robot Look: invalid robot position, it's out of range!");
	}

	try {
		if (getNode(position).type != nodeType::Robot) {
			throw new std::invalid_argument{ "Robot Look: invalid robot position, it's not a robot's position!" };
		}
	}
	catch (std::invalid_argument) {
		std::cout << "MapLook";
		throw;
	}

	int newPos[3];

	try {
		Transform(position, direction, newPos);
	}
	catch (std::invalid_argument) {
		std::cout << "MapLook";
		throw;
	}

	if (!ValidPos(newPos)) {
		return nodeType::Obstacle;
	}

	NodeObj node;

	try {
		node = getNode(newPos);
	}
	catch (std::invalid_argument) {
		std::cout << "MapLook";
		throw;
	}

	return node;
}

int Map::PlaceRobot(const int * position, unsigned int id)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("PlaceRobot: invalid robot position, it's out of range!");
	}

	try {
		if (getNode(position).type != nodeType::Free) {
			if (getNode(position).type == nodeType::Robot) {
				return 2;
			}
			else {
				return 1;
			}
		}
	}
	catch (std::invalid_argument& e) {
		std::cerr << e.what()<<std::endl;
		throw;
	}

	try {
		setNode(position, NodeObj(nodeType::Robot, id));
	}
	catch (std::invalid_argument& e) {
		std::cerr << e.what() << std::endl;
		throw;
	}
	return 0;
}

int Map::RemoveRobot(const int * position)
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("RemoveRobot: invalid robot position, it's out of range!");
	}

	if (getNode(position).type != nodeType::Robot) {
		return 1;
	}

	try {
		setNode(position, NodeObj(nodeType::Free));
	}
	catch (std::invalid_argument) {
		std::cout << "RemoveRobot";
		throw;
	}

	return 0;
}

void Map::Clean()
{
	if (dimensions == 2) {
		for (int y = size[1] - 1; y >= 0; --y) {
			for (int x = 0; x < size[0]; ++x) {
				if (static_cast<Map::NodeObj**>(MapArray)[x][y].type == nodeType::Robot) {
					static_cast<Map::NodeObj**>(MapArray)[x][y] = NodeObj(nodeType::Free);
				}
			}
		}
	}
	else if (dimensions == 3) {
		for (int y = size[1] - 1; y >= 0; --y) {
			for (int x = 0; x < size[0]; ++x) {
				for (int z = 0; z < size[2]; z++) {
					if (static_cast<Map::NodeObj***>(MapArray)[x][y][z].type == nodeType::Robot) {
						static_cast<Map::NodeObj***>(MapArray)[x][y][z] = NodeObj(nodeType::Free);
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
	/*for (int y = size[1] - 1; y >= 0; --y) {
		for (int x = 0; x < size[0]; ++x) {
			switch (((NodeObj**)MapArray)[x][y]) {
			case 0: std::cout << "-"; break;
			case 1: std::cout << "X"; break;
			case 2: std::cout << "O"; break;
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	std::cout << std::endl;*/
	
	IplImage* img = cvCreateImage(cvSize(size[0], size[1]), 8, 3);
	IplImage* big = cvCreateImage(cvSize(size[0] * 5, size[1] * 7), 8, 3);

	IplImage* redchannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* greenchannel = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage* bluechannel = cvCreateImage(cvGetSize(img), 8, 1);

	for (int y = 0; y < size[1]; ++y) {
		for (int x = 0; x < size[0]; ++x) {
			m_Access.lock();
			switch (((NodeObj**)MapArray)[x][y].type) {
			case 0:
				cvSetReal2D(redchannel, size[1] - y - 1, x, 0);
				cvSetReal2D(greenchannel, size[1] - y - 1, x, 0);
				cvSetReal2D(bluechannel, size[1] - y - 1, x, 0);
				break;
			case 1:
				cvSetReal2D(redchannel, size[1] - y - 1, x, 124);
				cvSetReal2D(greenchannel, size[1] - y - 1, x, 68);
				cvSetReal2D(bluechannel, size[1] - y - 1, x, 42);
				break;
			case 2:
				cvSetReal2D(redchannel, size[1] - y - 1, x, 255);
				cvSetReal2D(greenchannel, size[1] - y - 1, x, 0);
				cvSetReal2D(bluechannel, size[1] - y - 1, x, 0);
				break;
			}
			m_Access.unlock();
		}
	}

	cvMerge(bluechannel, greenchannel, redchannel, NULL, img);

	cvResize(img, big);


	cvNamedWindow("CurrentIteration");
	cvShowImage("CurrentIteration", big);
	cvWaitKey(1);
	cvReleaseImage(&img);
	cvReleaseImage(&big);
}

Map::Map()
{
	*size = { 0 };

	MapArray = nullptr;
}

Map::NodeObj Map::getNode(const int * position) const
{
	if (!ValidPos(position)) {
		throw new std::invalid_argument("GetNode: Invalid position!");
	}

	m_Access.lock();
	if (dimensions == 2) {
		NodeObj obj = static_cast<Map::NodeObj**>(MapArray)[position[0]][position[1]];
		m_Access.unlock();
		return obj;
	}
	else if(dimensions == 3){
		NodeObj obj = static_cast<Map::NodeObj***>(MapArray)[position[0]][position[1]][position[2]];
		m_Access.unlock();
		return obj;
	}
	
}

void Map::setNode(const int * position, const NodeObj& obj)
{
	
	if (!ValidPos(position)) {
		throw new std::invalid_argument("SetNode: Invalid position!");
	}
	

	m_Access.lock();
	if (dimensions == 2) {
		static_cast<Map::NodeObj**>(MapArray)[position[0]][position[1]] = obj;
	}
	else {
		static_cast<Map::NodeObj***>(MapArray)[position[0]][position[1]][position[2]] = obj;
	}
	m_Access.unlock();
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

bool Map::ValidPos(const int * position) const
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

void Map::DeleteMap()
{
	if (this->MapArray != nullptr) {
		if (dimensions == 2) {
			for (int i = 0; i < this->size[0]; ++i) {
				delete[]static_cast<NodeObj**>(this->MapArray)[i];
			}
			delete[]static_cast<NodeObj**>(this->MapArray);
		}
		else if (dimensions == 3) {
			for (int i = 0; i < size[0]; ++i) {
				for (int j = 0; j < size[1]; ++j) {
					delete[]static_cast<Map::NodeObj***>(this->MapArray)[i][j];
				}
				delete[]static_cast<Map::NodeObj***>(this->MapArray)[i];
			}
			delete[] static_cast<Map::NodeObj***>(this->MapArray);
		}
	}
}

Map::NodeObj::NodeObj():type(Map::nodeType::Free),id(0)
{
}

Map::NodeObj::NodeObj(nodeType type, unsigned int id): type(type), id(id)
{
}

Map::nodeType Map::NodeObj::GetType() const
{
	return type;
}

unsigned int Map::NodeObj::GetId() const
{
	return id;
}
