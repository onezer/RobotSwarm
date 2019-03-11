#define CATCH_CONFIG_MAIN
#include"catch.hpp"
#include "Map.h"
#include<atomic>

TEST_CASE("Setting up a 2D map, placing a robots and moving it around") {
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5 };
	std::atomic_int** mapArray = new std::atomic_int*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		mapArray[i] = new std::atomic_int[size[1]];
	}

	//Initializing the map's node values to Free
	for (int y = 0; y < size[1]; ++y) {
		for (int x = 0; x < size[0]; ++x) {
			mapArray[x][y] = Map::nodeType::Free;
		}
	}

	//Passing the array of the map, to the singleton handler object
	map->SetMap(mapArray, Map::mapType::twoD, size);

	//Placing a robot to the (2,2) position
	int position[2] = { 2,2 };
	REQUIRE(map->PlaceRobot(position) == 0); //PlaceRobot() return 0 if everything is OK

	//Asserting that only 1 robot is placed and on the right position
	for (int i = 0; i < size[0]; ++i) {
		for (int j = 0; j < size[1]; ++j) {
			if (j == 2 && i == 2) {
				REQUIRE(mapArray[i][j] == Map::nodeType::Robot);
			}
			else {
				REQUIRE(mapArray[i][j] == Map::nodeType::Free);
			}
		}
	}


	SECTION("Move the robot North") {
		REQUIRE(map->Move(position, Map::direction::North) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 3 && i == 2) {
					REQUIRE(mapArray[i][j] == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j] == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot South") {
		REQUIRE(map->Move(position, Map::direction::South) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 1 && i == 2) {
					REQUIRE(mapArray[i][j] == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j] == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot East") {
		REQUIRE(map->Move(position, Map::direction::East) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 2 && i == 3) {
					REQUIRE(mapArray[i][j] == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j] == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot West") {
		REQUIRE(map->Move(position, Map::direction::West) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 2 && i == 1) {
					REQUIRE(mapArray[i][j] == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j] == Map::nodeType::Free);
				}
			}
		}
	}
}