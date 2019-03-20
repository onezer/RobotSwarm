#define CATCH_CONFIG_MAIN
#include"catch.hpp"
#include "Map.h"
#include<atomic>

TEST_CASE("Setting up a 2D map, placing a robot and moving it around") {
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5 };
	Map::NodeObj** mapArray;
	mapArray = new Map::NodeObj*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((Map::NodeObj**)mapArray)[i] = new Map::NodeObj[size[1]];
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
	REQUIRE(map->PlaceRobot(position,0) == 0); //PlaceRobot() returns 0 if everything is OK
	REQUIRE(map->PlaceRobot(position,0) == 2); //PlaceRobot() returns 2 if the position is a robot

	//Asserting that only 1 robot is placed and on the right position
	for (int i = 0; i < size[0]; ++i) {
		for (int j = 0; j < size[1]; ++j) {
			if (j == 2 && i == 2) {
				REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
			}
			else {
				REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
			}
		}
	}


	SECTION("Move the robot North") {
		REQUIRE(map->Move(position, Map::direction::North) == 0); //Move() returns 0 if everything is OK

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 3 && i == 2) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot South") {
		REQUIRE(map->Move(position, Map::direction::South) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 1 && i == 2) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot East") {
		REQUIRE(map->Move(position, Map::direction::East) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 2 && i == 3) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Move the robot West") {
		REQUIRE(map->Move(position, Map::direction::West) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (j == 2 && i == 1) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Trying to move the robot out of the map") {
		REQUIRE(map->Move(position, Map::direction::West) == 0);
		REQUIRE(map->Move(position, Map::direction::West) == 0);
		REQUIRE(map->Move(position, Map::direction::West) == 2); //Move() returns 2 if it tries to move out of the map
	}

	SECTION("Placing obstacles and try to collide the robot with them") {
		mapArray[2][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::North) == 1); //Move() returns 1 if it would collide

		mapArray[2][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::South) == 1);

		mapArray[1][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::West) == 1);

		mapArray[3][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::East) == 1);
	}
}

TEST_CASE("Placing multiple robots on a 2D map, and moving them around") {
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5 };
	Map::NodeObj** mapArray;
	mapArray = new Map::NodeObj*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((Map::NodeObj**)mapArray)[i] = new Map::NodeObj[size[1]];
	}

	//Initializing the map's node values to Free
	for (int y = 0; y < size[1]; ++y) {
		for (int x = 0; x < size[0]; ++x) {
			mapArray[x][y] = Map::nodeType::Free;
		}
	}

	//Passing the array of the map, to the singleton handler object
	map->SetMap(mapArray, Map::mapType::twoD, size);

	//the robots' positions
	int positions[3][2] = { {0,0},{4,4},{2,2} };

	REQUIRE(map->PlaceRobot(positions[0],5) == 0);
	REQUIRE(map->PlaceRobot(positions[1],3) == 0);
	REQUIRE(map->PlaceRobot(positions[2],2) == 0);

	for (int i = 0; i < size[0]; ++i) {
		for (int j = 0; j < size[1]; ++j) {
			if(i==positions[0][0] && j == positions[0][1] || i == positions[1][0] && j == positions[1][1] || i == positions[2][0] && j == positions[2][1]){
				REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
			}
			else {
				REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
			}
		}
	}


	SECTION("Moving all 3 robots") {
		REQUIRE(map->Move(positions[0], Map::direction::North) == 0);
		REQUIRE(map->Move(positions[1], Map::direction::South) == 0);
		REQUIRE(map->Move(positions[2], Map::direction::East) == 0);

		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (i == 0 && j == 1 || i == 4 && j == 3 || i == 3 && j == 2) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Free);
				}
			}
		}
	}
	
	SECTION("Creating obstacles around the robots and collide them or leave the map") {
		//Around the 1st robot
		mapArray[0][1] = Map::nodeType::Obstacle;
		mapArray[1][0] = Map::nodeType::Obstacle;
		//Around the 2nd robot
		mapArray[4][3] = Map::nodeType::Obstacle;
		mapArray[3][4] = Map::nodeType::Obstacle;
		//Around the 3rd robot
		mapArray[2][1] = Map::nodeType::Obstacle;
		mapArray[1][2] = Map::nodeType::Obstacle;
		mapArray[2][3] = Map::nodeType::Obstacle;
		mapArray[3][2] = Map::nodeType::Obstacle;

		REQUIRE(map->Move(positions[0],Map::direction::North) == 1);
		REQUIRE(map->Move(positions[0], Map::direction::East) == 1);
		REQUIRE(map->Move(positions[0], Map::direction::West) == 2);
		REQUIRE(map->Move(positions[0], Map::direction::South) == 2);

		REQUIRE(map->Move(positions[1], Map::direction::North) == 2);
		REQUIRE(map->Move(positions[1], Map::direction::East) == 2);
		REQUIRE(map->Move(positions[1], Map::direction::West) == 1);
		REQUIRE(map->Move(positions[1], Map::direction::South) == 1);

		REQUIRE(map->Move(positions[2], Map::direction::North) == 1);
		REQUIRE(map->Move(positions[2], Map::direction::East) == 1);
		REQUIRE(map->Move(positions[2], Map::direction::West) == 1);
		REQUIRE(map->Move(positions[2], Map::direction::South) == 1);

		//Asserting the robots didn't move
		for (int i = 0; i < size[0]; ++i) {
			for (int j = 0; j < size[1]; ++j) {
				if (i == 0 && j == 0 || i == 4 && j == 4 || i == 2 && j == 2) {
					REQUIRE(mapArray[i][j].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[i][j].type != Map::nodeType::Robot);
				}
			}
		}
	}

}

TEST_CASE("Creating a 2D map, placing a robot, test the Look() method around the robot") {
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5 };
	Map::NodeObj** mapArray;
	mapArray = new Map::NodeObj*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((Map::NodeObj**)mapArray)[i] = new Map::NodeObj[size[1]];
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
	REQUIRE(map->PlaceRobot(position,0) == 0);

	SECTION("Every node around the robot is Free") {
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);
	}

	SECTION("Every node around the robot is Obstacle") {
		mapArray[2][3] = Map::nodeType::Obstacle;
		mapArray[2][1] = Map::nodeType::Obstacle;
		mapArray[1][2] = Map::nodeType::Obstacle;
		mapArray[3][2] = Map::nodeType::Obstacle;

		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
	}

	SECTION("Every node around the robot is Robot") {
		mapArray[2][3] = Map::nodeType::Robot;
		mapArray[2][1] = Map::nodeType::Robot;
		mapArray[1][2] = Map::nodeType::Robot;
		mapArray[3][2] = Map::nodeType::Robot;

		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Robot);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Robot);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Robot);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Robot);
	}

	SECTION("Different node-types around the robot") {
		mapArray[2][3] = Map::nodeType::Obstacle;
		mapArray[2][1] = Map::nodeType::Robot;
		mapArray[1][2] = Map::nodeType::Free;
		mapArray[3][2] = Map::nodeType::Robot;

		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Robot);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Robot);
	}

	SECTION("Obstacles and robots not in neighbouring nodes") {
		mapArray[1][0] = Map::nodeType::Obstacle;
		mapArray[4][4] = Map::nodeType::Robot;
		mapArray[3][3] = Map::nodeType::Robot;
		mapArray[1][4] = Map::nodeType::Obstacle;

		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);
	}
}

TEST_CASE("Creating a 3D array placing robots, moving them around") {
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5,5 };
	Map::NodeObj*** mapArray;
	mapArray = new Map::NodeObj**[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((Map::NodeObj***)mapArray)[i] = new Map::NodeObj*[size[1]];
		for (int j = 0; j < size[1]; ++j) {
			((Map::NodeObj***)mapArray)[i][j] = new Map::NodeObj[size[2]];
		}
	}

	//Initializing the map's node values to Free
	for (int x = 0; x < size[0]; ++x) {
		for (int y = 0; y < size[1]; ++y) {
			for (int z = 0; z < size[2]; ++z) {
				mapArray[x][y][z] = Map::nodeType::Free;
			}
		}
	}

	map->SetMap(mapArray, Map::mapType::threeD, size);

	SECTION("Moving 1 robot") {
		int position[3] = {2,2,2};
		REQUIRE(map->PlaceRobot(position,0) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position,Map::direction::Up) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::Down) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::East) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 3 && y == 2 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::West) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::North) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 3 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::South) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}
	}

	SECTION("Moving 2 robots") {
		int position[2][3] = { { 2,2,2 } , {3,3,3} };
		REQUIRE(map->PlaceRobot(position[0],0) == 0);
		REQUIRE(map->PlaceRobot(position[1],9) == 0);

		REQUIRE(map->Move(position[0], Map::direction::South) == 0);
		REQUIRE(map->Move(position[1], Map::direction::South) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 1 && z == 2 || x == 3 && y == 2 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position[0], Map::direction::North) == 0);
		REQUIRE(map->Move(position[1], Map::direction::North) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2 || x == 3 && y == 3 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position[0], Map::direction::Up) == 0);
		REQUIRE(map->Move(position[1], Map::direction::Up) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 3 || x == 3 && y == 3 && z == 4) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position[0], Map::direction::Down) == 0);
		REQUIRE(map->Move(position[1], Map::direction::Down) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2 || x == 3 && y == 3 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position[0], Map::direction::East) == 0);
		REQUIRE(map->Move(position[1], Map::direction::East) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 3 && y == 2 && z == 2 || x == 4 && y == 3 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}

		REQUIRE(map->Move(position[0], Map::direction::West) == 0);
		REQUIRE(map->Move(position[1], Map::direction::West) == 0);

		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; ++y) {
				for (int z = 0; z < size[2]; ++z) {
					if (x == 2 && y == 2 && z == 2 || x == 3 && y == 3 && z == 3) {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Robot);
					}
					else {
						REQUIRE(mapArray[x][y][z].type == Map::nodeType::Free);
					}
				}
			}
		}
	}

	SECTION("Testing the Look() method") {
		int position[3] = { 2,2,2 };

		REQUIRE(map->PlaceRobot(position,9) == 0);

		mapArray[3][2][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Free);

		mapArray[2][3][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Free);

		mapArray[2][2][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Free);

		mapArray[1][2][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Free);

		mapArray[2][1][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Free);

		mapArray[2][2][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::North).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::South).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Up).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::Down).type == Map::nodeType::Obstacle);
	}

	SECTION("Trying to collide with obstacle") {
		int position[3] = { 2,2,2 };
		mapArray[2][2][2] = Map::nodeType::Robot;

		mapArray[2][2][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::Up) == 1);

		mapArray[2][3][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::North) == 1);

		mapArray[3][2][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::East) == 1);

		mapArray[2][2][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::Down) == 1);

		mapArray[2][1][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::South) == 1);

		mapArray[1][2][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::West) == 1);
	}
}

TEST_CASE("Creating a hex map, placing robots and moving them around"){
	//Creating the map
	Map* map = Map::Instance();
	int size[] = { 5,5 };
	Map::NodeObj** mapArray;
	mapArray = new Map::NodeObj*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((Map::NodeObj**)mapArray)[i] = new Map::NodeObj[size[1]];
	}

	//Initializing the map's node values to Free
	for (int y = 0; y < size[1]; ++y) {
		for (int x = 0; x < size[0]; ++x) {
			mapArray[x][y] = Map::nodeType::Free;
		}
	}

	//Passing the array of the map, to the singleton handler object
	map->SetMap(mapArray, Map::mapType::hex, size);

	int position[2] = {2,2};

	SECTION("Placing a robot, moving it") {
		REQUIRE(map->PlaceRobot(position,0) == 0);

		REQUIRE(map->Move(position, Map::direction::NorthWest) == 0);
		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; y++) {
				if (x == 1 && y == 3) {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Free);
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::NorthEast) == 0);
		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; y++) {
				if (x == 1 && y == 4) {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Free);
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::SouthWest) == 0);
		REQUIRE(map->Move(position, Map::direction::SouthEast) == 0);
		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; y++) {
				if (x == 2 && y == 2) {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Free);
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::East) == 0);
		REQUIRE(map->Move(position, Map::direction::East) == 0);
		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; y++) {
				if (x == 4 && y == 2) {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Free);
				}
			}
		}

		REQUIRE(map->Move(position, Map::direction::West) == 0);
		REQUIRE(map->Move(position, Map::direction::West) == 0);
		for (int x = 0; x < size[0]; ++x) {
			for (int y = 0; y < size[1]; y++) {
				if (x == 2 && y == 2) {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Robot);
				}
				else {
					REQUIRE(mapArray[x][y].type == Map::nodeType::Free);
				}
			}
		}
	}

	SECTION("Testing the Look() method") {
		REQUIRE(map->PlaceRobot(position,5) == 0);
		
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[2][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[1][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[2][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[3][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Free);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[1][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Free);

		mapArray[3][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Look(position, Map::direction::NorthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::NorthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthEast).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::SouthWest).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::West).type == Map::nodeType::Obstacle);
		REQUIRE(map->Look(position, Map::direction::East).type == Map::nodeType::Obstacle);
	}

	SECTION("Trying to collide with obstacle") {
		mapArray[2][2] = Map::nodeType::Robot;

		mapArray[3][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::East) == 1);

		mapArray[1][2] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::West) == 1);

		mapArray[2][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::NorthEast) == 1);

		mapArray[1][3] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::NorthWest) == 1);

		mapArray[2][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::SouthWest) == 1);

		mapArray[3][1] = Map::nodeType::Obstacle;
		REQUIRE(map->Move(position, Map::direction::SouthEast) == 1);
	}
}