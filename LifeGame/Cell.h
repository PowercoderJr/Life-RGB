#pragma once
#include <Windows.h>
#include <vector>

#define RED_RACE_ID 0
#define RED_COLOR_RGB RGB(255, 0, 0)
#define GREEN_RACE_ID 1
#define GREEN_COLOR_RGB RGB(0, 255, 0)
#define BLUE_RACE_ID 2
#define BLUE_COLOR_RGB RGB(0, 0, 255)
#define NEUTRAL_RACE_ID 3
#define NEUTRAL_COLOR_RGB RGB(150, 150, 150)

class Cell
{
public:
	enum Race {RED=RED_RACE_ID, GREEN=GREEN_RACE_ID, BLUE=BLUE_RACE_ID, NEUTRAL=NEUTRAL_RACE_ID};
	static Cell* ProduceAvg(std::vector<Cell*> partners);
	Cell(COLORREF color);
	Cell(Cell* cell) : Cell(cell->brushColor) {}
	HBRUSH GetBrush();
	Race GetRace();
	bool Equals(Cell * cell);
	~Cell();
private:
	COLORREF brushColor;
	HBRUSH brush;
};