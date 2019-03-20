#pragma once
#include<list>
#include<thread>
#include<memory>
#include"Map.h"
#include"Robot.h"
#include"MapGenerator.h"
#include"iBehaviourFactory.h"
#include"iBehaviour.h"

class Controller
{
	Controller();

	static bool terminate;
	static Controller* s_instance;
	
	static std::mutex m_write;
	static std::mutex m_iter;
	static std::mutex m_terminate;
	static std::atomic_int threads_done[4];

	int maxThreads;
	int workerNum;
	std::list<std::unique_ptr<Robot>>* robotList;
	std::thread* workers;
	Map* map;
	MapGenerator* mapGenerator;

	std::unique_ptr<iBehaviourFactory> behaviourFactory;

	static void worker(int id, std::list<std::unique_ptr<Robot>>* robotList);
	static void iterationCB(unsigned int i);
	
	int wait;
	bool display;
	int robotStartPos[3];


	class Synchron {
		const unsigned int threadNum;
		std::atomic_int enter;
		std::atomic_int exit;
		std::atomic_int middle;
	public:
		Synchron(unsigned int threadNum);
		void Synch(bool wait=false);
	};

	std::unique_ptr<Synchron> synchObj;


public:
	static Controller* Instance();
	void WriteRobots() const;
	void AddRobot(int* position);
	void TerminateSimulation();
	void WaitForFinish();
	void StartSimulation(int* position, std::unique_ptr<iBehaviourFactory> behaviourFactory, bool display=false, int wait=500, unsigned int threadNum = 0);
	int getWorkerNum() const;

};