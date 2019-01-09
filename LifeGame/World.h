#pragma once
#include <Windows.h>
#include <vector>
#include <fstream>
#include <string>
#include "Cell.h"

using std::vector;

class World
{
public:
	World(int w, int h);
	World() : World(1, 1) {}
	void DrawGrid(HDC, RECT);
	void DrawCells(HDC, RECT);
	bool ReadFromFile(char* filepath);
	bool WriteToFile(char* filepath);
	void SetCell(int, int, Cell*);
	void Update();
	void ResetState();
	int GetRowsCount();
	int GetColsCount();
	int GetSquare();
	int GetTotalCellsCount();
	int GetCellsCountByRace(Cell::Race race);
	int GetGeneration();
	int GetCellsBorn();
	int GetCellsDied();
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