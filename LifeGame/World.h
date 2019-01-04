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
	void SetCell(int, int, Cell*);
	void Update();
	void ResetState();
	int GetRowsCount();
	int GetColsCount();
	int GetTotalCellsCount();
	int GetCellsCountByRace(Cell::Race race);
	~World();
private:
	int rowsCount;
	int colsCount;
	vector<vector<Cell*>> matrix;

	int generation;
	int square;
	int totalCellsCount;
	int racesCellsCount[RACES_COUNT];
	int cellsBorn;
	int cellsDied;

	void CloneMatrix(const vector<vector<Cell*>>, vector<vector<Cell*>>*);
	void DeleteMatrix(vector<vector<Cell*>>*);
};