#include "Controller.h"
#include<algorithm>

Controller* Controller::s_instance;

int Controller::max_threads;
int Controller::worker_num;
std::atomic_int Controller::threads_done[4];
std::mutex Controller::m_write;
std::mutex Controller::m_iter;
std::mutex Controller::m_terminate;

bool Controller::terminate;

Controller::Controller()
{
	max_threads = std::thread::hardware_concurrency() == 1 ? 2 : std::thread::hardware_concurrency();
	worker_num = max_threads - 1;

	*threads_done = { 0 };

	iteration = 0;

	map = Map::Instance();
	mapGenerator = MapGenerator::Instance();

	workers = new std::thread[worker_num];

	robotList = new std::list<std::unique_ptr<Robot>>[worker_num];

	terminate = false;
}

void Controller::worker(int id, std::list<std::unique_ptr<Robot>>* robotList)
{
	enum Task { Look, Compute, Move };

	unsigned int i = 0;

	auto LCM = [](std::list<std::unique_ptr<Robot>>* robotList, Task task) {
		for (auto it = robotList->begin(); it != robotList->end(); it++) {
			switch (task) {
			case Look: (**it).Look(); break;
			case Compute: (**it).Compute(); break;
			case Move: (**it).Move(); break;
			}
		}
	};

	++threads_done[3];
	while (threads_done[3] != worker_num && !terminate);

	while (!terminate) {
		++i;

		if (id == 0 && !terminate) {
			iterationCB(i);
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

void Controller::iterationCB(unsigned int i)
{
	if (i ==1 || i == 200) {
		Controller::Instance()->AddRobot(Controller::Instance()->robotStartPos);
	}

    //m_write.lock();
	Map::Instance()->DisplayMap();
	//m_write.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

void Controller::AddRobot(int* position)
{
	unsigned int count = Robot::getCount();

	
	try {
		int success = map->PlaceRobot(position);
		if (success == 0) {
			robotList[count % worker_num].push_back(std::make_unique<Robot>(count, position));
		}
		else if (success == 1) {
			terminate = true;
			m_write.lock();
			std::cout << "Robot placed on obstacle\n";
			m_write.unlock();
		}
		else if (success == 2) {
			terminate = true;
			m_write.lock();
			std::cout << "Robot placed on robot\n";
			m_write.unlock();
		}
	}
	catch (std::exception& e) {
		m_write.lock();
		std::cout << e.what();
		m_write.unlock();
	}
	
}

void Controller::TerminateSimulation()
{
	terminate = true;
}

void Controller::WaitForFinish()
{
	for (int i = 0; i < worker_num; ++i) {
		workers[i].join();
	}
}

void Controller::StartSimulation(int* position)
{
	terminate = false;

	std::memcpy(robotStartPos,position,map->getDimensions()*sizeof(int));

	std::fill(threads_done, threads_done + 4, 0);

	for (int i = 0; i < worker_num; ++i) {
		if(robotList[i].size())
			robotList[i].clear();
	}

	for (int i = 0; i < worker_num; ++i) {
		workers[i] = std::thread(&Controller::worker, i, &(robotList[i]));
		/*m_write.lock();
		std::cout << "Worker #" << i << " created\n";
		m_write.unlock();*/
	}
}

int Controller::getWorkerNum() const
{
	return worker_num;
}