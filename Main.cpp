#include<iostream>
#include<memory>
#include<list>
#include<thread>
#include<vector>
#include<mutex>
#include<chrono>
#include<atomic>
#include<time.h>
#include"Controller.h"
#include"MapGenerator.h"
#include"iBehaviour.h"
#include"iBehaviourFactory.h"
#include"Filling.hpp"
#include"Map.h"

int main() { 

	Controller* controller = Controller::Instance();
	MapGenerator* mapGenerator = MapGenerator::Instance();
	Map* map = Map::Instance();

	int size[2] = { 40,40 };
	int pos[2] = { 40,15 };
	mapGenerator->GenerateMap(Map::mapType::twoD, size, false, 12335);

	auto start = std::chrono::steady_clock::now();
	for (int x = 0; x < 1; ++x) {
		map->Clean();

		controller->StartSimulation(pos, Filling::Factory(), true , 800, 1);

		controller->WaitForFinish();

		std::cout << "robot count: " << Robot::getCount() << std::endl;
		std::cout << x << ". simuation\n";
	}
	auto end = std::chrono::steady_clock::now();
	
	std::cout << "DONE\n" << "Simulation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n"; 

}