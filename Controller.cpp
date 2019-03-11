#include "Controller.h"


Controller* Controller::s_instance;

int Controller::max_threads;
int Controller::worker_num;
std::atomic_int* Controller::threads_done;
std::thread* Controller::workers;
std::mutex Controller::m_write;
std::mutex Controller::m_iter;
std::list<Robot>* Controller::robotList;
bool Controller::terminate;

Controller::Controller()
{
	max_threads = std::thread::hardware_concurrency() == 1 ? 2 : std::thread::hardware_concurrency();
	worker_num = max_threads - 1;

	threads_done = new std::atomic_int[4];
	for (int i = 0; i < 4; ++i) {
		threads_done[i] = 0;
	}

	map = Map::Instance();
	mapGenerator = MapGenerator::Instance();

	workers = new std::thread[worker_num];

	robotList = new std::list<Robot>[worker_num];

	terminate = false;
}

void Controller::worker(int id, std::list<Robot>* robotList)
{
	enum Task { Look, Compute, Move };

	int i = 0;

	auto LCM = [](std::list<Robot>* robotList, Task task) {
		for (auto it = robotList->begin(); it != robotList->end(); it++) {
			switch (task) {
			case Look: (*it).Look(); break;
			case Compute: (*it).Compute(); break;
			case Move: (*it).Move(); break;
			}
		}
	};

	++threads_done[3];
	while (threads_done[3] != worker_num && !terminate);

	while (!terminate) {
		++i;

		if (m_iter.try_lock()) {
			iterationCB(i);
			m_iter.unlock();
		}

		++threads_done[0];
		while (threads_done[0] != worker_num && !terminate);
		threads_done[3] = 0;

		if (!terminate) {
			LCM(robotList, Look);
		}
		

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage1\n";
		m_write.unlock();*/

		++threads_done[1];
		while (threads_done[1] != worker_num && !terminate);
		threads_done[0] = 0;

		if (!terminate) {
			LCM(robotList, Compute);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage2\n";
		m_write.unlock();*/

		++threads_done[2];
		while (threads_done[2] != worker_num && !terminate);
		threads_done[1] = 0;

		if (!terminate) {
			LCM(robotList, Move);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage3\n";
		m_write.unlock();*/

		++threads_done[3];
		while (threads_done[3] != worker_num && !terminate);
		threads_done[2] = 0;
		
	}
}

void Controller::iterationCB(int i)
{
	//s_instance->AddRobots(1);
	//m_write.lock();
	//std::cout << "Iteration: " << i << std::endl;
	/*std::cout << "Robot count: " << Robot::getCount() << std::endl;
	for (int j = 0; j < worker_num; ++j) {
		std::cout << "Thread #" << i << " robot count: " << robotList[j].size() << std::endl;
	}*/
	//m_write.unlock();
}


Controller * Controller::Instance()
{
	if (!s_instance)
		s_instance = new Controller;
	return s_instance;
}

void Controller::WriteRobots() const
{
	/*
	m_write.lock();
	for (int i = 0; i < worker_num; ++i) {
		int j = 0;
		for (auto it = robotList[i].begin(); it != robotList[i].end(); it++) {
			std::cout << "Robot[" << i << ":" << j << "] Look: " << (*it).look << std::endl;
			std::cout << "Robot[" << i << ":" << j << "] Compute: " << (*it).compute << std::endl;
			std::cout << "Robot[" << i << ":" << j << "] Move: " << (*it).move << std::endl;
			j++;
		}
	}
	m_write.unlock(); */
}

void Controller::AddRobots(unsigned int number)
{
	int* position;
	int dimensions = map->getDimensions();

	for (int i = 0; i < number; ++i) {
		unsigned int count = Robot::getCount();

		position = new int[dimensions];
		for (int i = 0; i < dimensions; ++i) {
			position[i] = 0;
		}

		robotList[count % worker_num].push_back(*(new Robot(count, position)));
	}
}

void Controller::TerminateSimulation()
{
	terminate = true;
}

std::thread* Controller::StartSimulation()
{
	for (int i = 0; i < worker_num; ++i) {
		workers[i] = std::thread(&Controller::worker, i, &(robotList[i]));
		m_write.lock();
		std::cout << "Worker #" << i << " created\n";
		m_write.unlock();
	}

	return workers;
}

int Controller::getWorkerNum() const
{
	return worker_num;
}