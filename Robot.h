#pragma once
#include <atomic>
#include "Map.h"
#include<iostream>

class Robot
{
	static std::_Atomic_uint count;
	unsigned int ID;
	Map* map;
	Controller* controller;

	int* position;

	void Move(Map::direction);
	void Look(Map::direction);

	int lookData[10];

public:

	void Look();
	void Compute();
	void Move();
	static const std::_Atomic_uint getCount();
	unsigned int getID();
	Robot(unsigned int id, int* position);
	~Robot();
};

