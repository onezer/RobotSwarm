#pragma once
#include<list>
#include<thread>
#include<memory>
#include"Map.h"
#include"Robot.h"
#include"MapGenerator.h"

class Controller
{
	Controller();

	static int max_threads;
	static int worker_num;
	static std::atomic_int* threads_done;
	static std::thread* workers;
	static std::mutex m_write;
	static std::mutex m_iter;
	static std::list<std::shared_ptr<Robot>>* robotList;
	static bool CBDone;
	static std::mutex m_terminate;

	Map* map;
	MapGenerator* mapGenerator;
	static bool terminate;
	static Controller* s_instance;

	static void worker(int id, std::list<std::shared_ptr<Robot>>* robotList);
	static void iterationCB(int i);
	
	int iteration;
	int robotStartPos[3];
public:
	static Controller* Instance();
	void WriteRobots() const;
	void AddRobot(int* position);
	void TerminateSimulation();
	std::thread* StartSimulation(int* position);
	int getWorkerNum() const;

};