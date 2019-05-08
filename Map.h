#pragma once
#include <mutex>

//Singleton class th is responsible for managing the map
//The robots use its methods to look and move around the map
class Map
{
	Map();
	void* MapArray;
	static Map* s_instance;
	int size[3];
	int dimensions;
	
	mutable std::mutex m_Move;

public:
	enum nodeType {Free, Obstacle, Robot, Nest, Resource};
	enum direction{North, South, West, East, Up, Down, NorthWest, NorthEast, SouthWest, SouthEast};
	enum mapType { twoD, threeD, hex };

	struct NodeObj {
		unsigned int id;
		nodeType type;
		NodeObj();
		NodeObj(nodeType type, unsigned int id = 0);
		nodeType GetType()const;
		unsigned int GetId()const;
	};

	static Map* Instance();
	void SetMap(void* MapArray, mapType maptype, int* size);
	int getDimensions() const;
	int getType() const;
	const int* getSize() const;
	std::string getXML() const;

	int Move(int* position, direction direction);
	NodeObj Look(int* position, direction direction);
	int PlaceRobot(const int* position, unsigned int);
	int RemoveRobot(const int* position);

	void Clean();
	void* Recycle(int* size); //Returning the allocated array pointer, if it's the same size as the parameter
	void DisplayMap() const; //Displaying the map with OpenCV every iteration

private:
	mapType maptype;
	bool ValidPos(const int* position) const;
	Map::NodeObj getNode(const int* position) const;
	void setNode(const int* position, const Map::NodeObj& obj);
	void Transform(int* position, direction direction, int* newPosition) const;
	bool ValidDir(direction direction) const;
	void DeleteMap();
};