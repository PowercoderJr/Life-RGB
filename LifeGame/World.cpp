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
	cells = vector<vector<Cell*>>(rowsCount, vector<Cell*>(colsCount, nullptr));
	generation = 0;
	totalCellsCount = 0;
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
			if (cells[i][j] != nullptr)
			{
				// TODO: закрашивать клетки внутри границ или поверх?
				cellRect.top = i * (rc.bottom - rc.top) / rowsCount;
				cellRect.bottom = (i + 1) * (rc.bottom - rc.top) / rowsCount + 1;
				cellRect.left = j * (rc.right - rc.left) / colsCount;
				cellRect.right = (j + 1) * (rc.right - rc.left) / colsCount + 1;
				FillRect(dc, &cellRect, cells[i][j]->GetBrush());
			}
}

void World::Update()
{
	vector<vector<Cell*>> cellsCopy;
	CloneMatrix(cells, &cellsCopy);
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

int World::GetColsCount()
{
	return colsCount;
}

int World::GetTotalCellsCount()
{
	return totalCellsCount;
}

int World::GetCellsCountByRace(Cell::Race race)
{
	return racesCellsCount[race];
}

int World::GetRowsCount()
{
	return rowsCount;
}

World::~World()
{
	DeleteMatrix(&cells);
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

void World::SetCell(int i, int j, Cell* cell)
{
	if (cells[i][j] == nullptr)
	{
		if (cell != nullptr)
		{
			++totalCellsCount;
			++racesCellsCount[cell->GetRace()];
		}
	}
	else
	{
		--racesCellsCount[cells[i][j]->GetRace()];
		delete cells[i][j];
		if (cell != nullptr)
			++racesCellsCount[cell->GetRace()];
		else
			--totalCellsCount;
	}
	cells[i][j] = cell;
}

void World::ReadPosition(ifstream& fin, int& x, int& y)
{
	string str;
	fin >> str;
	x = atoi(str.c_str());
	fin >> str;
	y = atoi(str.c_str());
	x += colsCount / 2;
	y += rowsCount / 2;
	x = Cycled(x, colsCount);
	y = Cycled(y, rowsCount);
}

/*int World::LoadPattern(string path)
{
	ifstream fin;
	fin.open(path, ifstream::in);
	if (fin.fail())
	{
		return 1;
	}
	string str;
	int x, y, i, j;
	char c;
	while (str != "#P")
		fin >> str;
	ReadPosition(fin, x, y);
	i = x, j = y;
	c = fin.get();
	while (c != EOF && fin.good())
	{
		while (c != EOF && fin.good() && c != '#')
		{
			if (!(c = fin.get()) || c == EOF)
				break;
			switch (c)
			{
			case '*':
				cells[i++][j] = 1;
				totalCellsCount += 1;
				i = Cycled(i, rowsCount);
				break;
			case '.':
				cells[i++][j] = 0;
				i = Cycled(i, rowsCount);
				break;
			case '\n':
				i = x;
				j = Cycled(++j, colsCount);
				break;
			default:
				break;
			}
		}
		if (c == '#')
		{
			c = fin.get();
			ReadPosition(fin, x, y);
			i = x, j = y;
			c = fin.get();
		}
	}
	fin.close();
	return 0;
}*/
