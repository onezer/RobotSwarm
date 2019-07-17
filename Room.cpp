#include "Room.h"
#include"Map.h"


Room::Room()
{
}

Room::Room(const std::list<Coord>& roomTiles, void * map)
{
	tiles = roomTiles;
	roomSize = roomTiles.size();

	for each(Coord tile in tiles) {
		for (int x = tile.tileX - 1; x <= tile.tileX + 1; x++) {
			for (int y = tile.tileY - 1; y <= tile.tileY + 1; y++) {
				if (x == tile.tileX || y == tile.tileY) {
					if ( ((Map::NodeObj**)map)[x][y].type == Map::nodeType::Obstacle) {
						edgeTiles.push_back(tile);
					}
				}
			}
		}
	}
}

void Room::ConnectRooms(std::shared_ptr<Room> roomA, std::shared_ptr<Room> roomB)
{
	roomA->connectedRooms.push_back(roomB);
	roomB->connectedRooms.push_back(roomA);
}

bool Room::IsConnected(const Room & otherRoom) const
{
	for each(std::shared_ptr<Room> room in connectedRooms)
	{
		if (*room == otherRoom)
			return true;
	}

	return false;
}

bool Room::operator==(const Room & other) const
{
	if (roomSize != other.roomSize)
		return false;

	return (tiles == other.tiles);
}


Room::~Room()
{
	tiles.clear();
	edgeTiles.clear();
	connectedRooms.clear();
}
