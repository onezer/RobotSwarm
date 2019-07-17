#pragma once
#include<list>
#include"Coord.h"
#include<memory>


class Room
{
public:
	std::list<Coord> tiles;
	std::list<Coord> edgeTiles;
	std::list<std::shared_ptr<Room>> connectedRooms;
	int roomSize;

	Room();

	Room(const std::list<Coord>& roomTiles, void* map);
	static void ConnectRooms(std::shared_ptr<Room> roomA, std::shared_ptr<Room> roomB);
	bool IsConnected(const Room& otherRoom) const;

	bool operator== (const Room& other) const;
	~Room(); 
};

