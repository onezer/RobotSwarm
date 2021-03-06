#include "Controller.h"
#include<algorithm>
#include<vector>

Controller* Controller::s_instance;


Controller::Controller()
{
	maxThreads = std::thread::hardware_concurrency() == 1 ? 2 : std::thread::hardware_concurrency();
	workerNum = maxThreads - 1;

	map = Map::Instance();
	mapGenerator = MapGenerator::Instance();


	terminate = false;
}

//Thread function
void Controller::worker(int id, std::list<std::unique_ptr<Robot>>* robotList)
{
	enum Task { Look, Compute, Move };

	unsigned int i = 0;
	//Calling the assigned robots' methods
	auto LCM = [](std::list<std::unique_ptr<Robot>>* robotList, Task task) {
		for (auto it = robotList->begin(); it != robotList->end(); it++) {
			switch (task) {
			case Look: (**it).Look(); break;
			case Compute: (**it).Compute(); break;
			case Move: (**it).Move(); break;
			}
		}
	};

	//Controller* controller = Controller::Instance();

	++threads_done[3];
	while (threads_done[3] != workerNum && !terminate);

	while (!terminate) {

		//Synchronization of threads
		++threads_done[0];
		while (threads_done[0] != workerNum && !terminate);
		threads_done[3] = 0;

		if (!terminate) {
			LCM(robotList, Look);
		}
		

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage1\n";
		m_write.unlock();*/

		++threads_done[1];
		while (threads_done[1] != workerNum && !terminate);
		threads_done[0] = 0;

		if (!terminate) {
			LCM(robotList, Compute);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage2\n";
		m_write.unlock();*/

		++threads_done[2];
		while (threads_done[2] != workerNum && !terminate);
		threads_done[1] = 0;

		if (!terminate) {
			LCM(robotList, Move);
		}

		/*m_write.lock();
		std::cout << "Thread #" << id << " is done with stage3\n";
		m_write.unlock();*/

		++threads_done[3];
		while (threads_done[3] != workerNum && !terminate);
		threads_done[2] = 0;

		if (id == 0 && !terminate) {
			iterationCB(i);
		}

		++i;
	}
}

void Controller::iterationCB(unsigned int i)
{
	std::cout << "\nIteration: " << i << std::endl;

	if (map->SpaceNum() == 0) {
		TerminateSimulation();
		std::cout << "Map filled!"<< std::endl;
	}

	if (true|| i % 2 == 1) {
		AddRobot(robotStartPos);
	}
	else {
		map->PlaceNest(robotStartPos);
	}


    //m_write.lock();
	if (display) {
		Map::Instance()->DisplayMap();
		std::this_thread::sleep_for(std::chrono::milliseconds(Controller::Instance()->wait));
	}
	//m_write.unlock();

	FileWriter::Instance()->PushToBuffer(*currentIteration);
	currentIteration = new Iteration(i+1);
}


Controller * Controller::Instance()
{
	if (!s_instance)
		s_instance = new Controller;
	return s_instance;
}

void Controller::AddRobot(int* position)
{
	unsigned int count = Robot::getCount();

	
	try {
		int success;
		try {
			success = map->PlaceRobot(position, count);
		}
		catch (std::invalid_argument e) {
			std::cerr << "AddRobot: " << e.what() << std::endl;
		}
		if (success == 0) {
			if (!terminate) {
				robotList[count % workerNum].push_back(std::make_unique<Robot>(count, position, behaviourFactory->CreateBehaviour(count)));

			}
			
		}
		else if (success == 1) {
			TerminateSimulation();
			m_write.lock();
			std::cout << "Terminated: Robot placed on obstacle\n";
			m_write.unlock();
			
		}
		/*else if (success == 2) {
			TerminateSimulation();
			m_write.lock();
			std::cout << "Terminated: Robot placed on robot\n";
			m_write.unlock();
			
		}*/
	}
	catch (std::invalid_argument e) {
		m_write.lock();
		std::cerr << e.what() << ":AddRobot\n";
		m_write.unlock();
	}
	
}

void Controller::TerminateSimulation()
{
	terminate = true;
	FileWriter::Instance()->StopWriting();
}

void Controller::WaitForFinish()
{
	for (int i = 0; i < workerNum; ++i) {
		workers[i].join();
	}
	writer->join();
}

void Controller::StartSimulation(std::unique_ptr<iBehaviourFactory> bFactory, std::string fileName, bool display, int wait, unsigned int threadNum)
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
	
	for (int i = 0; i < workerNum; ++i) {
		if (robotList[i].size())
			robotList[i].clear();
	}

	behaviourFactory = std::move(bFactory);
	terminate = false;
	this->display = display;
	this->wait = wait;

	Coord position = mapGenerator->GetStartCoord();

	robotStartPos[0] = position.tileX;
	robotStartPos[1] = position.tileY;
	robotStartPos[2] = position.tileZ;

	currentIteration = new Iteration(0);

	for (int i = 0; i < workerNum; ++i) {
		workers[i] = std::thread(&Controller::worker, this, i, &(robotList[i]));
		/*m_write.lock();
		std::cout << "Worker #" << i << " created\n";
		m_write.unlock();*/
	}

	FileWriter* writerInstance = FileWriter::Instance();

	writer = new std::thread(&FileWriter::StartWriting, writerInstance, fileName);
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
