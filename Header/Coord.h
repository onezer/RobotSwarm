#pragma once
struct Coord {
	int tileX;
	int tileY;
	int tileZ;

	Coord(int x, int y, int z = 0) : tileX{ x }, tileY{ y }, tileZ{z} {}
	Coord(){}
};