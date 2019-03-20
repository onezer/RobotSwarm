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


class GoingSpiral : public iBehaviour {
	Map::direction nextMove;

	class GoingSpiralFactory : public iBehaviourFactory {
	public:
		std::unique_ptr<iBehaviour> CreateBehaviour(unsigned int id) {
			return std::make_unique<GoingSpiral>(id);
		}
	};

public:
	GoingSpiral(unsigned int id)
	{
		this->id = id;
	}

	std::vector<Map::direction> Look() {
		std::vector<Map::direction> ret = std::vector<Map::direction>();
		ret.push_back(Map::direction::North);
		ret.push_back(Map::direction::East);
		ret.push_back(Map::direction::West);
		ret.push_back(Map::direction::South);
		return ret;
	}

	void Compute(std::unordered_map<Map::direction, Map::NodeObj> lookData) {
		if (lookData[Map::direction::North].GetType() == Map::nodeType::Free && nextMove != Map::direction::South) {
			nextMove = Map::direction::North;
		}
		else if (lookData[Map::direction::East].GetType() == Map::nodeType::Free  && nextMove != Map::direction::West) {
			nextMove = Map::direction::East;
		}
		else if (lookData[Map::direction::South].GetType() == Map::nodeType::Free  && nextMove != Map::direction::North) {
			nextMove = Map::direction::South;
		}
		else if (lookData[Map::direction::West].GetType() == Map::nodeType::Free  && nextMove != Map::direction::East) {
			nextMove = Map::direction::West;
		}
	}


	std::pair<bool, Map::direction> Move() {
		return std::pair<bool, Map::direction>(true, nextMove);
	}

	static std::unique_ptr<iBehaviourFactory> Factory() {
		return std::make_unique<GoingSpiralFactory>();
	}
};



int main() { 

	Controller* controller = Controller::Instance();
	MapGenerator* mapGenerator = MapGenerator::Instance();
	Map* map = Map::Instance();

	int size[2] = { 300,30000 };
	//int size[2] = { 10,7 };

	

	/*std::atomic_int** mapArray = new std::atomic_int*[size[0]];
	for (int i = 0; i < size[0]; ++i) {
		((std::atomic_int**)mapArray)[i] = new std::atomic_int[size[1]];
	}

	for (int i = 0; i < size[0]; ++i) {
		for (int j = 0; j < size[1]; ++j) {
			mapArray[i][j] = 0;
		}
	}*/

	//map->SetMap(mapArray, Map::mapType::twoD, size);

	int pos[2] = { 100,25 };
	//int pos[2] = { 0,0 };

	auto start = std::chrono::steady_clock::now();
	mapGenerator->GenerateMap(Map::mapType::twoD, size, false, 12335);
	for (int x = 0; x < 20; ++x) {
		map->Clean();

		controller->StartSimulation(pos, GoingSpiral::Factory(), false,200,8);

		controller->WaitForFinish();
		//std::cout << x << ". simuation\n";
	}


	//std::cout << std::endl << Robot::getCount() << " robots\n";

	auto end = std::chrono::steady_clock::now();

	

	std::cout << "DONE\n" << "Simulation time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n"; 

	std::getchar();
}