#include "Robot.h"
#include"Controller.h"


std::_Atomic_uint Robot::count;

Robot::Robot(unsigned int id, int* position) : ID{ id }
{
	map = Map::Instance();
	controller = Controller::Instance();
	for (int i = 0; i < 10; ++i) {
		lookData[i] = -1;
	}

	std::memcpy(this->position,position, Map::Instance()->getDimensions()*sizeof(int));

	++count;
}


Robot::~Robot()
{
	--count;
}

void Robot::Move(Map::direction direction)
{
	try {
		int result = map->Move(position, direction);
		if (result != 0) {
			controller->TerminateSimulation();
			std::cout << "Simulation terminated: Robot with ID:" << ID << " collided!\n";
		}
	}
	catch (std::invalid_argument& e) {
		std::cout << e.what();
	}
}

void Robot::Look(Map::direction direction)
{
	try {
		lookData[direction] = map->Look(position, direction);
	}
	catch (std::invalid_argument& e) {
		std::cout << e.what();
	}
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

	if (lookData[Map::direction::North] == 0 && nextMove != Map::direction::South) {
		nextMove = Map::direction::North;
	} 
	else if (lookData[Map::direction::East] == 0 && nextMove != Map::direction::West) {
		nextMove = Map::direction::East;
	}
	else if (lookData[Map::direction::South] == 0 && nextMove != Map::direction::North) {
		nextMove = Map::direction::South;
	}
	else if (lookData[Map::direction::West] == 0 && nextMove != Map::direction::East) {
		nextMove = Map::direction::West;
	}
}

void Robot::Move()
{
	if (nextMove >= 0) {
		Move(nextMove);
	}
}

const std::_Atomic_uint Robot::getCount()
{
	return count;
}

unsigned int Robot::getID()
{
	return ID;
}
