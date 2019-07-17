#pragma once
#include<list>
#include<thread>
#include<memory>
#include"Map.h"
#include"Robot.h"
#include"MapGenerator.h"
#include"iBehaviourFactory.h"
#include"iBehaviour.h"
#include"FileWriter.h"
#include"Coord.h"

//Singleton class that is repsonsible for controlling the simulation
class Controller
{
	Controller();

	bool terminate;
	static Controller* s_instance;
	
	std::mutex m_write;
	std::mutex m_iter;
	std::mutex m_terminate;
	std::atomic_int threads_done[4];

	int maxThreads;
	int workerNum;
	std::list<std::unique_ptr<Robot>>* robotList;
	std::thread* workers;
	std::thread* writer;
	Map* map;
	MapGenerator* mapGenerator;

	std::unique_ptr<iBehaviourFactory> behaviourFactory;

	void worker(int id, std::list<std::unique_ptr<Robot>>* robotList);
	void iterationCB(unsigned int i);
	
	int wait;
	bool display;
	int robotStartPos[3];


	class Synchron {
		const unsigned int threadNum;
		std::atomic_uint enter;
		std::atomic_uint exit;
		std::atomic_uint middle;
	public:
		Synchron(unsigned int threadNum);
		void Synch(bool wait=false);
	};

	std::unique_ptr<Synchron> synchObj;


public:
	
	static Controller* Instance();
	void AddRobot(int* position);
	void TerminateSimulation();
	void WaitForFinish();
	void StartSimulation(std::unique_ptr<iBehaviourFactory> behaviourFactory, std::string fileName, bool display=false, int wait=500, unsigned int threadNum = 0);
	int getWorkerNum() const;
	Iteration* currentIteration;
};