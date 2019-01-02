#include "World.h"
#include <vector>

using std::vector;

inline int Cycled(int input, int base)
{
	if (input >= 0) return input % base; else return base + input;
}

World::World(int width, int height)
{
	this->width = width;
	this->height = height;
	cells = vector<vector<Cell*>>(height, vector<Cell*>(width, nullptr));
	cellsBuf = vector<vector<Cell*>>(cells);
	isPaused = true;
	generation = 0;
	totalCellsCount = 0;
}

void World::DrawGrid(HDC dc, RECT rc)
{
	for (int i = 0; i<height; i++)
	{
		int y = i * (rc.bottom - rc.top) / height;
		MoveToEx(dc, rc.left, y, NULL);
		LineTo(dc, rc.right, y);
	}
	for (int i = 0; i<width; i++)
	{
		int x = i * (rc.right - rc.left) / width;
		MoveToEx(dc, x, rc.top, NULL);
		LineTo(dc, x, rc.bottom);
	}
}

void World::DrawCells(HDC dc, RECT rc)
{
	//HBRUSH hBrush = CreateSolidBrush(RED_COLOR_RGB);
	//HBRUSH oBrush = (HBRUSH)SelectObject(dc, hBrush);
	RECT cellRect;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (cells[i][j] != nullptr)
			{
				cellRect.left = i * (rc.right - rc.left) / width;
				cellRect.right = (i + 1) * (rc.right - rc.left) / width + 1;
				cellRect.top = j * (rc.bottom - rc.top) / height;
				cellRect.bottom = (j + 1) * (rc.bottom - rc.top) / height + 1;
				FillRect(dc, &cellRect, cells[i][j]->GetBrush());
			}
}

void World::Update()
{
	CloneGrid(cells, &cellsBuf);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			vector<Cell*> neighbours = vector<Cell*>();
			for (int k = -1; k <= 1; k++)
				for (int m = -1; m <= 1; m++)
					if (!(m == 0 && k == 0))
					{
						Cell* neighbour = cells[Cycled(i + k, height)][Cycled(j + m, width)];
						if (neighbour != nullptr)
							neighbours.push_back(neighbour);
					}

			if (cells[i][j] == nullptr)
			{
				if (neighbours.capacity() == 3)
				{
					cellsBuf[i][j] = Cell::ProduceAvg(neighbours);
					++totalCellsCount;
				}
			}
			else
			{
				if (neighbours.capacity() < 2 || neighbours.capacity() > 3)
				{
					delete cellsBuf[i][j];
					cellsBuf[i][j] = nullptr;
					--totalCellsCount;
				}
			}
		}
	}
	CloneGrid(cellsBuf, &cells);
	generation += 1;
}

bool World::IsPaused()
{
	return isPaused;
}

void World::SetIsPaused(bool isPaused)
{
	this->isPaused = isPaused;
}

World::~World()
{
	for (int i = 0; i < height; ++i)
		cells[i].clear();
}

void World::CloneGrid(const vector<vector<Cell*>> src, vector<vector<Cell*>>* dst)
{
	int width = src[0].size();
	int height = src.size();
	*dst = vector<vector<Cell*>>(height, vector<Cell*>(width, nullptr));
	for (int i = 0; i < height; ++i)
		for (int j = 0; j < width; ++j)
			if (src[i][j] != nullptr)
				(*dst)[i][j] = new Cell(src[i][j]);
}

void World::SetCell(RECT rc, int x, int y, Cell* cell)
{
	// TODO?: int <-> float
	int i = x * width / (rc.right - rc.left);
	int j = y * height / (rc.bottom - rc.top);
	if (cells[i][j] != nullptr && !cells[i][j]->Equals(cell))
	{
		if (cell == nullptr)
			--totalCellsCount;
		else
			++totalCellsCount;
	}
	if (cells[i][j] != nullptr)
		delete cells[i][j];
	cells[i][j] = cell;

}

void World::ReadPosition(ifstream& fin, int& x, int& y)
{
	string str;
	fin >> str;
	x = atoi(str.c_str());
	fin >> str;
	y = atoi(str.c_str());
	x += width / 2;
	y += height / 2;
	x = Cycled(x, width);
	y = Cycled(y, height);
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
				i = Cycled(i, height);
				break;
			case '.':
				cells[i++][j] = 0;
				i = Cycled(i, height);
				break;
			case '\n':
				i = x;
				j = Cycled(++j, width);
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
