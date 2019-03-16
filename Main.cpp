#include<iostream>
#include<list>
#include<thread>
#include<mutex>
#include<chrono>
#include<atomic>
#include<time.h>
#include"Controller.h"
#include"MapGenerator.h"
int main() { 

	Controller* controller = Controller::Instance();
	MapGenerator* mapGenerator = MapGenerator::Instance();
	Map* map = Map::Instance();

	int size[2] = { 300,75 };
	//int size[2] = { 10,7 };

	

	/*std::atomic_int** mapArray = new std::atomic_int*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((std::atomic_int**)mapArray)[i] = new std::atomic_int[size[1]];
	}

	for (int i = 0; i < size[0]; ++i) {
		for (int j = 0; j < size[1]; ++j) {
			mapArray[i][j] = 0;
		}
	}

	map->SetMap(mapArray, Map::mapType::twoD, size);*/

	int pos[2] = { 100,25 };
	//int pos[2] = { 0,0 };

	//auto start = std::chrono::steady_clock::now();

	for (int x = 0; x < 50000; ++x) {
		mapGenerator->GenerateMap(Map::mapType::twoD, size, false, 12335);
		std::thread* workers = controller->StartSimulation(pos);

		int worker_num = controller->getWorkerNum();

		for (int i = 0; i < worker_num; ++i) {
			workers[i].join();
		}
		//std::cout << x << ". simuation\n";
	}


	//std::cout << std::endl << Robot::getCount() << " robots\n";

	//auto end = std::chrono::steady_clock::now();

	

	//std::cout << "DONE\n" << "Simulation time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds\n"; 
}