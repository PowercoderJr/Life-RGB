#include "World.h"
#include <vector>

using std::vector;

inline int Cycled(int input, int base)
{
	if (input >= 0) return input % base; else return base + input;
}

World::World(int rowsCount, int colsCount)
{
	this->rowsCount = rowsCount;
	this->colsCount = colsCount;
	this->square = this->rowsCount * this->colsCount;
	matrix = vector<vector<Cell*>>(rowsCount, vector<Cell*>(colsCount, nullptr));
	ResetState();
}

void World::DrawGrid(HDC dc, RECT rc)
{
	for (int i = 0; i < rowsCount; i++)
	{
		int y = i * (rc.bottom - rc.top) / rowsCount;
		MoveToEx(dc, rc.left, y, NULL);
		LineTo(dc, rc.right, y);
	}
	for (int i = 0; i < colsCount; i++)
	{
		int x = i * (rc.right - rc.left) / colsCount;
		MoveToEx(dc, x, rc.top, NULL);
		LineTo(dc, x, rc.bottom);
	}
}

void World::DrawCells(HDC dc, RECT rc)
{
	RECT cellRect;
	for (int i = 0; i < rowsCount; i++)
		for (int j = 0; j < colsCount; j++)
			if (matrix[i][j] != nullptr)
			{
				// TODO: закрашивать клетки внутри границ или поверх?
				cellRect.top = i * (rc.bottom - rc.top) / rowsCount;
				cellRect.bottom = (i + 1) * (rc.bottom - rc.top) / rowsCount + 1;
				cellRect.left = j * (rc.right - rc.left) / colsCount;
				cellRect.right = (j + 1) * (rc.right - rc.left) / colsCount + 1;
				FillRect(dc, &cellRect, matrix[i][j]->GetBrush());
			}
}

void World::Update()
{
	vector<vector<Cell*>> cellsCopy;
	CloneMatrix(matrix, &cellsCopy);
	for (int i = 0; i < rowsCount; i++)
	{
		for (int j = 0; j < colsCount; j++)
		{
			vector<Cell*> neighbours = vector<Cell*>();
			for (int k = -1; k <= 1; k++)
				for (int m = -1; m <= 1; m++)
					if (!(m == 0 && k == 0))
					{
						Cell* neighbour = cellsCopy[Cycled(i + k, rowsCount)][Cycled(j + m, colsCount)];
						if (neighbour != nullptr)
							neighbours.push_back(neighbour);
					}

			if (cellsCopy[i][j] == nullptr)
			{
				if (neighbours.capacity() == 3)
				{
					SetCell(i, j, Cell::ProduceAvg(neighbours));
				}
			}
			else
			{
				if (neighbours.capacity() < 2 || neighbours.capacity() > 3)
				{
					SetCell(i, j, nullptr);
				}
			}
		}
	}
	DeleteMatrix(&cellsCopy);
	++generation;
}

void World::ResetState()
{
	generation = 0;
	totalCellsCount = 0;
	for (int i = 0; i < RACES_COUNT; ++i)
		racesCellsCount[i] = 0;
	cellsBorn = 0;
	cellsDied = 0;
}

int World::GetColsCount()
{
	return colsCount;
}

int World::GetSquare()
{
	return square;
}

int World::GetTotalCellsCount()
{
	return totalCellsCount;
}

int World::GetCellsCountByRace(Cell::Race race)
{
	return racesCellsCount[race];
}

int World::GetGeneration()
{
	return generation;
}

int World::GetCellsBorn()
{
	return cellsBorn;
}

int World::GetCellsDied()
{
	return cellsDied;
}

int World::GetRowsCount()
{
	return rowsCount;
}

World::~World()
{
	DeleteMatrix(&matrix);
}

void World::CloneMatrix(const vector<vector<Cell*>> src, vector<vector<Cell*>>* dst)
{
	int colsCount = src[0].size();
	int rowsCount = src.size();
	*dst = vector<vector<Cell*>>(rowsCount, vector<Cell*>(colsCount, nullptr));
	for (int i = 0; i < rowsCount; ++i)
		for (int j = 0; j < colsCount; ++j)
			if (src[i][j] != nullptr)
				(*dst)[i][j] = new Cell(src[i][j]);
}

void World::DeleteMatrix(vector<vector<Cell*>>* matrix)
{
	for (int i = 0; i < rowsCount; ++i)
	{
		for (int j = 0; j < colsCount; ++j)
			delete (*matrix)[i][j];
		(*matrix)[i].clear();
		(*matrix)[i].shrink_to_fit();
	}
	(*matrix).clear();
	(*matrix).shrink_to_fit();
}

bool World::ReadFromFile(char * filepath)
{
	bool result = false;
	char filepathBufbuf[32];
	HANDLE fin = CreateFile(filepath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (fin == INVALID_HANDLE_VALUE)
		return false;

	try
	{
		char buf[32];
		DWORD x;
		ReadFile(fin, buf, 3, &x, NULL);
		if (x == 3)
		{
			rowsCount = std::atoi(buf);
			ReadFile(fin, buf, 3, &x, NULL);
		}
		if (x == 3)
		{
			colsCount = std::atoi(buf);
			square = rowsCount * colsCount;
			DeleteMatrix(&matrix);
			matrix = vector<vector<Cell*>>(rowsCount, vector<Cell*>(colsCount, nullptr));
			for (int i = 0; i < rowsCount; ++i)
			{
				for (int j = 0; j < colsCount; ++j)
				{
					ReadFile(fin, buf, 8, &x, NULL);
					int color = std::atoi(buf);
					if (color >= 0)
						matrix[i][j] = new Cell(color);
				}
			}

			ReadFile(fin, buf, 8, &x, NULL);
			generation = std::atoi(buf);
			ReadFile(fin, buf, 8, &x, NULL);
			totalCellsCount = std::atoi(buf);
			for (int i = 0; i < RACES_COUNT; ++i)
			{
				ReadFile(fin, buf, 8, &x, NULL);
				racesCellsCount[i] = std::atoi(buf);
			}
			ReadFile(fin, buf, 8, &x, NULL);
			cellsBorn = std::atoi(buf);
			ReadFile(fin, buf, 8, &x, NULL);
			cellsDied = std::atoi(buf);

			result = true;
		}
	}
	catch (...)
	{
		;
	}

	CloseHandle(fin);
	return result;
}

bool World::WriteToFile(char * filepath)
{
	bool result = false;
	HANDLE fout = CreateFileA(filepath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
	if (fout == INVALID_HANDLE_VALUE)
		return false;

	try
	{
		char buf[32];
		sprintf_s(buf, "%03d%03d", rowsCount, colsCount);
		DWORD x;
		WriteFile(fout, buf, strlen(buf), &x, NULL);
		for (int i = 0; i < rowsCount; ++i)
		{
			for (int j = 0; j < colsCount; ++j)
			{
				sprintf_s(buf, "%08d", matrix[i][j] == nullptr ? -1 : matrix[i][j]->GetBrushColor());
				WriteFile(fout, buf, strlen(buf), &x, NULL);
			}
		}
		sprintf_s(buf, "%08d", generation);
		WriteFile(fout, buf, strlen(buf), &x, NULL);
		sprintf_s(buf, "%08d", totalCellsCount);
		WriteFile(fout, buf, strlen(buf), &x, NULL);
		for (int i = 0; i < RACES_COUNT; ++i)
		{
			sprintf_s(buf, "%08d", racesCellsCount[i]);
			WriteFile(fout, buf, strlen(buf), &x, NULL);
		}
		sprintf_s(buf, "%08d", cellsBorn);
		WriteFile(fout, buf, strlen(buf), &x, NULL);
		sprintf_s(buf, "%08d", cellsDied);
		WriteFile(fout, buf, strlen(buf), &x, NULL);
		result = true;
	}
	catch (...)
	{
		;
	}
	CloseHandle(fout);
	return result;
}

void World::SetCell(int i, int j, Cell* cell)
{
	if (matrix[i][j] == nullptr)
	{
		if (cell != nullptr)
		{
			++totalCellsCount;
			++racesCellsCount[cell->GetRace()];
			++cellsBorn;
		}
	}
	else
	{
		--racesCellsCount[matrix[i][j]->GetRace()];
		++cellsDied;
		delete matrix[i][j];
		if (cell != nullptr)
		{
			++racesCellsCount[cell->GetRace()];
			++cellsBorn;
		}
		else
		{
			--totalCellsCount;
		}
	}
	matrix[i][j] = cell;
}
