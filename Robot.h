#pragma once
#include<atomic>
class Robot
{
	static std::_Atomic_uint count;

	unsigned int ID;


public:
	int look;
	int compute;
	int move;
	void Look();
	void Compute();
	void Move();
	static const std::_Atomic_uint getCount();
	unsigned int getID();
	Robot(unsigned int id);
	~Robot();
};

