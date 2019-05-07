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
#include"FileWriter.h"
#include<string>

int main() { 

	/*Controller* controller = Controller::Instance();
	MapGenerator* mapGenerator = MapGenerator::Instance();
	Map* map = Map::Instance();

	int size[2] = { 50,40 };
	int pos[2] = { 40,15 };
	mapGenerator->GenerateMap(Map::mapType::twoD, size, false, 12335);

	auto start = std::chrono::steady_clock::now();
	for (int x = 0; x < 1; ++x) {
		map->Clean();

		controller->StartSimulation(pos, Filling::Factory(), true, 300, 1);

		controller->WaitForFinish();

		std::cout << "robot count: " << Robot::getCount() << std::endl;
		std::cout << x << ". simuation\n";
	}
	auto end = std::chrono::steady_clock::now();
	
	std::cout << "DONE\n" << "Simulation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";*/

	FileWriter* writer = FileWriter::Instance();
	MapGenerator* mapGenerator = MapGenerator::Instance();
	Map* nap = Map::Instance();

	int size[2] = { 3,3 };
	mapGenerator->GenerateMap(Map::mapType::twoD, size, false, 10);

	RobotPosition pos1(1, 2, 3, 1, RobotPosition::Type::Creation);
	RobotPosition pos2(2, 1, 3, 2, RobotPosition::Type::Creation);

	Iteration* it1 = new Iteration(1);
	it1->positions->push_back(pos1);
	it1->positions->push_back(pos2);

	Iteration* it2 = new Iteration(2);
	pos1.x = 2;
	pos2.y = 2;
	pos1.type = RobotPosition::Type::Move;
	pos2.type = RobotPosition::Type::Move;
	it2->positions->push_back(pos1);
	it2->positions->push_back(pos2);

	writer->PushToBuffer(*it1);
	writer->PushToBuffer(*it2);
	
	std::thread writing(&FileWriter::StartWriting, writer, "testing");
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	writer->StopWriting();

	writing.join();
}