#pragma once
#include"Robot.h"
#include<atomic>
#include<list>
#include<mutex>
#include"Map.h"
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
	static std::list<Robot>* robotList;
	Map* map;
	MapGenerator* mapGenerator;
	static bool terminate;
	static Controller* s_instance;

	static void worker(int id, std::list<Robot>* robotList);
	static void iterationCB(int i);
	
public:
	static Controller* Instance();
	void WriteRobots() const;
	void AddRobots(unsigned int number);
	void TerminateSimulation();
	std::thread* StartSimulation();
	int getWorkerNum() const;

};

