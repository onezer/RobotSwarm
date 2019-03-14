#include "Robot.h"
#include"Controller.h"


std::_Atomic_uint Robot::count;

Robot::Robot(unsigned int id, int* position) : ID{ id }, position{position}
{
	map = Map::Instance();
	controller = Controller::Instance();
	for (int i = 0; i < 10; ++i) {
		lookData[i] = -1;
	}

	++count;
}


Robot::~Robot()
{
	delete[] position;
	--count;
}

void Robot::Move(Map::direction direction)
{
	int result = map->Move(position, direction);
	if (result != 0) {
		controller->TerminateSimulation();
		std::cout << "Simulation terminated: Robot with ID:" << ID << " collided!\n";
	}
}

void Robot::Look(Map::direction direction)
{
	lookData[direction] = map->Look(position, direction);
}

void Robot::Look()
{
	Look(Map::direction::North);
	Look(Map::direction::East);
	Look(Map::direction::West);
	Look(Map::direction::South);
}

void Robot::Compute()
{
	std::cout << position[0] << " " << position[1] << std::endl;

	if (lookData[Map::direction::North] == 0 && lookData[Map::direction::West] == 1) {
		nextMove = Map::direction::North;
	} 
	else if (lookData[Map::direction::East] == 0 && lookData[Map::direction::North] == 1) {
		nextMove = Map::direction::East;
	}
	else if (lookData[Map::direction::South] == 0 && lookData[Map::direction::East] == 1) {
		nextMove = Map::direction::South;
	}
	else if (lookData[Map::direction::West] == 0 && lookData[Map::direction::South] == 1) {
		nextMove = Map::direction::West;
	}

	if (position[0] == 1 && position[1] == 0) {
		controller->TerminateSimulation();
	}
}

void Robot::Move()
{
	Move(nextMove);
}

const std::_Atomic_uint Robot::getCount()
{
	return count;
}

unsigned int Robot::getID()
{
	return ID;
}
