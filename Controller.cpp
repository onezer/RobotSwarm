#include "Controller.h"
#include<algorithm>
#include<vector>

Controller* Controller::s_instance;

std::mutex Controller::m_write;
std::mutex Controller::m_iter;
std::mutex Controller::m_terminate;
std::atomic_int Controller::threads_done[4];

bool Controller::terminate=false;

Controller::Controller()
{
	maxThreads = std::thread::hardware_concurrency() == 1 ? 2 : std::thread::hardware_concurrency();
	workerNum = maxThreads - 1;

	map = Map::Instance();
	mapGenerator = MapGenerator::Instance();

	

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

	Controller* controller = Controller::Instance();

	++threads_done[3];
	while (threads_done[3] != controller->workerNum && !terminate);

	while (!terminate) {
		++i;

		if (id == 0 && !terminate) {
			iterationCB(i);
		}

		++threads_done[0];
		while (threads_done[0] != controller->workerNum && !terminate);
		threads_done[3] = 0;

		if (!terminate) {
			LCM(robotList, Look);
		}
		

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage1\n";
		m_write.unlock();*/

		++threads_done[1];
		while (threads_done[1] != controller->workerNum && !terminate);
		threads_done[0] = 0;

		if (!terminate) {
			LCM(robotList, Compute);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage2\n";
		m_write.unlock();*/

		++threads_done[2];
		while (threads_done[2] != controller->workerNum && !terminate);
		threads_done[1] = 0;

		if (!terminate) {
			LCM(robotList, Move);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage3\n";
		m_write.unlock();*/

		++threads_done[3];
		while (threads_done[3] != controller->workerNum && !terminate);
		threads_done[2] = 0;
	}
}

void Controller::iterationCB(unsigned int i)
{
	if (i % 2 == 1) {
		Controller::Instance()->AddRobot(Controller::Instance()->robotStartPos);
	}

    //m_write.lock();
	if (Controller::Instance()->display) {
		Map::Instance()->DisplayMap();
		std::this_thread::sleep_for(std::chrono::milliseconds(Controller::Instance()->wait));
	}
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

void Controller::AddRobot(int* position)
{
	unsigned int count = Robot::getCount();

	
	try {
		int success = map->PlaceRobot(position,count);
		if (success == 0) {
			robotList[count % workerNum].push_back(std::make_unique<Robot>(count, position, behaviourFactory->CreateBehaviour(count)));
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
		std::cerr << e.what() << ":AddRobot\n";
		m_write.unlock();
	}
	
}

void Controller::TerminateSimulation()
{
	terminate = true;
}

void Controller::WaitForFinish()
{
	for (int i = 0; i < workerNum; ++i) {
		workers[i].join();
	}
}

void Controller::StartSimulation(int* position, std::unique_ptr<iBehaviourFactory> bFactory, bool display, int wait, unsigned int threadNum)
{
	if (threadNum) {
		workerNum = threadNum;
	}
	synchObj = std::make_unique<Synchron>(workerNum);
	std::fill(threads_done, threads_done + 4, 0);

	if (workers == nullptr) {
		workers = new std::thread[workerNum];
	}

	if (robotList == nullptr) {
		robotList = new std::list<std::unique_ptr<Robot>>[workerNum];
	}
	

	behaviourFactory = std::move(bFactory);
	terminate = false;
	this->display = display;
	this->wait = wait;

	std::memcpy(robotStartPos,position,map->getDimensions()*sizeof(int));

	for (int i = 0; i < workerNum; ++i) {
		if(robotList[i].size())
			robotList[i].clear();
	}

	for (int i = 0; i < workerNum; ++i) {
		workers[i] = std::thread(&Controller::worker, i, &(robotList[i]));
		/*m_write.lock();
		std::cout << "Worker #" << i << " created\n";
		m_write.unlock();*/
	}
}

int Controller::getWorkerNum() const
{
	return workerNum;
}

Controller::Synchron::Synchron(unsigned int threadNum):threadNum(threadNum), enter(0), exit(0), middle(0)
{
}

void Controller::Synchron::Synch(bool wait)
{
	++enter;
	while (enter != threadNum) {
		if (wait) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	exit = 0;

	++middle;
	while (middle != threadNum) {
		if (wait) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	enter = 0;

	++exit;
	while (exit != threadNum) {
		if (wait) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	middle = 0;
}
