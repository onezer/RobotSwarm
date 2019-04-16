#pragma once
#include<queue>
#include<list>
#include<memory>
#include<unordered_map>

struct RobotPosition {
	int x;
	int y;
	int z;

	unsigned int id;

	RobotPosition(int x, int y, int z, unsigned int id);
};

struct Iteration {
	int num;

	std::list<RobotPosition>* positions;
	Iteration(int num);
	Iteration(Iteration&& theOther);
	Iteration(const Iteration& theOther);
	~Iteration();
	friend void swap(Iteration& first, Iteration& second);
	Iteration& operator=(Iteration theOther);
};


class FileWriter
{
	FileWriter();
	static FileWriter* s_instance;
	std::queue<Iteration> buffer;
	bool writing;
	std::unordered_map<std::string, int> fileNameDictionary;

	std::string ConstructFileName(std::string name);

public:
	static FileWriter* Instance();
	
	void PushToBuffer(Iteration& i);
	void StartWriting(const std::string& fileName);
	void StopWriting();
};


