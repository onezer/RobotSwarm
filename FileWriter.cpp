#include "FileWriter.h"
#include "Map.h"
#include<utility>
#include<string>
#include<fstream>
#include<iostream>

FileWriter* FileWriter::s_instance;

FileWriter::FileWriter() :writing{ false }
{
	
}

FileWriter * FileWriter::Instance()
{
	if (s_instance == nullptr) {
		s_instance = new FileWriter();
	}
	return s_instance;
}

void FileWriter::PushToBuffer(Iteration& i)
{
	buffer.push(std::move(i));
}


bool FileExists(const std::string& name) {
	std::ifstream f(name.c_str());
	bool ret = f.good();
	f.close();
	return ret;
}

std::string IterationToXML(const Iteration& it) {
	std::string retString;

	retString = "<iteration num=\"" + std::to_string(it.num) + "\">\n";

	for each (RobotPosition robot in *it.positions)
	{
		retString += "<robot>\n";
		retString += "<id>" + std::to_string(robot.id) + "</id>\n";
		retString += "<x>" + std::to_string(robot.x) + "</x>\n";
		retString += "<y>" + std::to_string(robot.y) + "</y>\n";
		retString += "<z>" + std::to_string(robot.z) + "</z>\n";
		retString += "<type>";
		switch (robot.type) {
		case RobotPosition::Type::Creation: retString += "creation"; break;
		case RobotPosition::Type::Move: retString += "move"; break;
		case RobotPosition::Type::Collision: retString += "collision"; break;
		}
		retString += "</type>\n";
		retString += "</robot>\n";
	}

	retString += "</iteration>\n";

	return retString;
}

std::string FileWriter::ConstructFileName(std::string name) {
	int fileNameNum = 0;

	if (fileNameDictionary.find(name) != fileNameDictionary.end()) {
		fileNameDictionary.at(name) += 1;
		fileNameNum = fileNameDictionary.at(name);
	}
	else {
		fileNameDictionary.insert(std::pair<std::string, int>(name, 0));
	}

	std::string fileName = name + std::to_string(fileNameNum) + ".xml";
	return fileName;
}

std::string MapToXML() {
	Map* map = Map::Instance();

	int sizex = map->getSize()[0];
	int sizey = map->getSize()[1];
	int sizez = map->getSize()[2];

	std::string retString("<size>\n");

	retString += "<x>" + std::to_string(sizex) + "</x>\n";
	retString += "<y>" + std::to_string(sizey) + "</y>\n";
	retString += "<z>" + std::to_string(sizez) + "</z>\n";
	retString += "</size>\n";
	retString += map->getXML();

	return retString;
}

void FileWriter::StartWriting(const std::string& name)
{
	writing = true;

	std::string fileName = ConstructFileName(name);

	while (FileExists(fileName)) {
		fileName = ConstructFileName(name);
	}


	std::ofstream file;
	file.open(fileName);
	file << "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n";
	file << "<simulation name=\"" + name + "\">\n";
	
	file << "<map>\n";
	file << MapToXML();
	file << "</map>\n";
	

	while (writing || buffer.size() > 0) {
		if (buffer.size() > 0) {
			file << IterationToXML(buffer.front());
			buffer.pop();
		}
	}
	file << "</simulation>";
	
	file.close();
}

void FileWriter::StopWriting()
{
	writing = false;
}

Iteration::Iteration(int num) :num{ num }, positions{ new std::list<RobotPosition>() }
{
}

Iteration::Iteration(Iteration && theOther):positions{theOther.positions}, num{theOther.num}
{
	theOther.positions = nullptr;
}

Iteration::Iteration(const Iteration & theOther):positions{ new std::list<RobotPosition>(*theOther.positions) }
{
}

Iteration::~Iteration()
{
	if (positions != nullptr) {
		positions->clear();
		delete positions;
	}
}

Iteration & Iteration::operator=(Iteration theOther)
{
	swap(*this, theOther);

	return *this;
}

void swap(Iteration& first, Iteration& second)
{
	using std::swap;

	swap(first.positions, second.positions);
	
	int tmp = first.num;
	first.num = second.num;
	second.num = tmp;

}

RobotPosition::RobotPosition(int x, int y, int z, unsigned int id, Type type):x{x},y{y},z{z},id{id}, type{type}
{
}
