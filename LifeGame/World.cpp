#include "World.h"

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
	cellsCounter = 0;
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
	//HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
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
	cellsBuf = cells;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int around = 0;
			for (int k = -1; k <= 1; k++)
				for (int m = -1; m <= 1; m++)
					if (!(m == 0 && k == 0))
						if (cells[Cycled(i + k, height)][Cycled(j + m, width)] != nullptr)
							++around;

			if (cells[i][j] != nullptr)
			{
				if (around != 2 && around != 3)
				{
					cellsBuf[i][j] = 0;
					cellsCounter -= 1;
				}
			}
			else
			{
				if (around == 3)
				{
					cellsBuf[i][j] = new Cell(0, 0, 0);
					++cellsCounter;
				}
			}
		}
	}
	cells = cellsBuf;
	generation += 1;
}

World::~World()
{
	for (int i = 0; i < height; ++i)
		cells[i].clear();
}

void World::SetCell(RECT rc, int x, int y, Cell* cell)
{
	// TODO?: int <-> float
	int i = x * width / (rc.right - rc.left);
	int j = y * height / (rc.bottom - rc.top);
	if (cells[i][j] != nullptr && !cells[i][j]->Equals(cell))
	{
		if (cell == nullptr)
			--cellsCounter;
		else
			++cellsCounter;
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
				cellsCounter += 1;
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
