#include "Robot.h"
#include"Controller.h"
#include<vector>
#include<unordered_map>
#include<vector>


std::_Atomic_uint Robot::count;

Robot::Robot(unsigned int id, int* position, std::unique_ptr<iBehaviour> behaviour) : id{ id }, behaviour{std::move(behaviour)}
{
	map = Map::Instance();
	controller = Controller::Instance();

	std::memcpy(this->position,position, Map::Instance()->getDimensions()*sizeof(int));

	++count;

	RobotPosition filePos(position[0], position[1], position[2], id, RobotPosition::Type::Creation);
	Controller::Instance()->currentIteration->positions->push_back(filePos);
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
			std::cout << "Simulation terminated: Robot with ID:" << id << " collided!\n";
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << ":move\n";
		throw;
	}
}

void Robot::Look(Map::direction direction)
{
	try {
		lookData.insert_or_assign(direction, map->Look(position, direction));
	}
	catch (std::exception& e) {
		std::cerr << e.what() << ":Look\n";
		throw;
	}
}

void Robot::Look()
{
	std::vector<Map::direction> look = behaviour->Look();
	for (int i = 0; i < look.size(); ++i) {
		Look(look[i]);
	}
}

void Robot::Compute()
{
	behaviour->Compute(lookData);
}

void Robot::Move()
{
	std::pair<bool, Map::direction> moveData = behaviour->Move();
	if (moveData.first) {
		Move(moveData.second);
	}
}

const std::_Atomic_uint Robot::getCount()
{
	return count;
}

unsigned int Robot::getID()
{
	return id;
}
