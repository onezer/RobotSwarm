#pragma once
#include <atomic>
#include<iostream>
#include<atomic>
#include "Map.h"
#include"iBehaviour.h"
#include"iBehaviourFactory.h"

class Controller;
class Map;

//Represents the robots in the simulation
class Robot
{
	static std::_Atomic_uint count;
	unsigned int id;
	Map* map;
	Controller* controller;

	int position[3];

	void Move(Map::direction);
	void Look(Map::direction);

	std::unordered_map<Map::direction, Map::NodeObj> lookData;

	std::unique_ptr<iBehaviour> behaviour;

public:

	void Look();
	void Compute();
	void Move();
	static const std::_Atomic_uint getCount();
	unsigned int getID();
	Robot(unsigned int id, int* position, std::unique_ptr<iBehaviour> behaviour);
	~Robot();
};