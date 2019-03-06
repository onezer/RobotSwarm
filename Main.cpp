#include<iostream>
#include<list>
#include<thread>
#include<mutex>
#include<chrono>
#include<atomic>
#include<time.h>
#include"Robot.h"
#include"Controller.h"
#include"Map.h"

int main() { 

	Controller* controller = Controller::Instance();

	controller->AddRobots(1100);

	auto start = std::chrono::steady_clock::now();

	std::thread* workers = controller->StartSimulation();

	int worker_num = controller->getWorkerNum();

	//std::this_thread::sleep_for(std::chrono::milliseconds(700));

	//controller->TerminateSimulation();

	for (int i = 0; i < worker_num; ++i) {
		workers[i].join();
	}

	auto end = std::chrono::steady_clock::now();

	controller->WriteRobots();

	//std::cout << "Robot count: " << Robot::getCount() << std::endl;

	std::cout << "DONE\n" << "Simulation time: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds\n"; 

	std::getchar();
}