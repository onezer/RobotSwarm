#pragma once
#include"iBehaviour.h"
#include"Map.h"

class iBehaviourFactory {
public:
	virtual std::unique_ptr<iBehaviour> CreateBehaviour(unsigned int id)=0;
};