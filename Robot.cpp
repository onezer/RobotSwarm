#include "Robot.h"


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
	//implement robot algorithm here
}

void Robot::Compute()
{
	//implement robot algorithm here
}

void Robot::Move()
{
	//implement robot algorithm here
}

const std::_Atomic_uint Robot::getCount()
{
	return count;
}

unsigned int Robot::getID()
{
	return ID;
}
