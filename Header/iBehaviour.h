#pragma once
#include "Map.h"
#include<vector>
#include<unordered_map>

//The behaviour algorithm of the robots is separated from the Robot class
//The behaviour class that implements the algorithm must implement this interface
class iBehaviour
{
protected:
	unsigned int id;
public:
	virtual std::vector<Map::direction> Look() = 0;
	virtual void Compute(std::unordered_map<Map::direction,Map::NodeObj>) = 0;
	virtual std::pair<bool, Map::direction> Move() = 0;
};