#pragma once
#include <Windows.h>

class Cell
{
public:
	Cell(byte r, byte g, byte b);
	HBRUSH GetBrush();
	bool Equals(Cell * cell);
	~Cell();
private:
	byte r, g, b;
	HBRUSH brush;
};