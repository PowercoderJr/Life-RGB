#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <vector>
#include <fstream>
#include <string>
#include "Cell.h"

using std::vector;
using std::ifstream;
using std::string;

class World
{
public:
	World(int w, int h);
	World() : World(1, 1) {}
	void DrawGrid(HDC, RECT);
	void DrawCells(HDC, RECT);
	void ReadPosition(ifstream&, int&, int&);
	//int LoadPattern(string);
	void SetCell(RECT, int, int, Cell*);
	void Update();
	bool IsPaused();
	void SetIsPaused(bool isPaused);
	~World();
private:
	int height;
	int width;
	vector<vector<Cell*>> cells;
	vector<vector<Cell*>> cellsBuf;

	bool isPaused;
	int generation;
	int square;
	int totalCellsCount;
	int redCellsCount;
	int greenCellsCount;
	int blueCellsCount;
	int neutralCellsCount;

	void CloneGrid(const vector<vector<Cell*>> src, vector<vector<Cell*>>* dst);
};