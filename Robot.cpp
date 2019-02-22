#include "Robot.h"
#include<atomic>
#include"Controller.h"

std::_Atomic_uint Robot::count;

Robot::Robot(unsigned int id) : ID{ id }, look{0}, compute{0}, move{0}
{
	++count;
}


Robot::~Robot()
{
	--count;
}

void Robot::Look()
{
	++look;
}

void Robot::Compute()
{
	++compute;
}

void Robot::Move()
{
	++move;
}

const std::_Atomic_uint Robot::getCount()
{
	return count;
}

unsigned int Robot::getID()
{
	return ID;
}
