cmake_minimum_required (VERSION 3.15)
project (RobotSwarm)

set(SOURCES Source/FileWriter.cpp Source/Main.cpp Source/Map.cpp Source/Robot.cpp Source/Controller.cpp Source/MapGenerator.cpp Source/Room.cpp Header/FileWriter.h Header/Map.h Header/catch.hpp Header/MapGenerator.h Header/Robot.h Header/Room.h Header/SINGLE.hpp Header/Controller.h Header/Coord.h Header/FileWriter.h Header/iBehaviour.h Header/iBehaviourFactory.h)

add_executable(RobotSwarm ${SOURCES})

target_include_directories(RobotSwarm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Header)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Header)

set(CMAKE_SUPPRESS_REGENERATION true)