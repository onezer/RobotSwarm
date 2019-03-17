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

	static bool terminate;
	static Controller* s_instance;
	static int max_threads;
	static int worker_num;
	static std::atomic_int threads_done[4];
	static std::mutex m_write;
	static std::mutex m_iter;
	static bool CBDone;
	static std::mutex m_terminate;

	std::list<std::unique_ptr<Robot>>* robotList;
	std::thread* workers;
	Map* map;
	MapGenerator* mapGenerator;

	static void worker(int id, std::list<std::unique_ptr<Robot>>* robotList);
	static void iterationCB(int i);
	
	int iteration;
	int robotStartPos[3];

public:
	static Controller* Instance();
	void WriteRobots() const;
	void AddRobot(int* position);
	void TerminateSimulation();
	void WaitForFinish();
	void StartSimulation(int* position);
	int getWorkerNum() const;

};