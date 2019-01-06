#include "Cell.h"

using std::vector;

Cell::Cell(COLORREF brushColor)
{
	this->brushColor = brushColor;
	this->brush = CreateSolidBrush(this->brushColor);
}

HBRUSH Cell::GetBrush()
{
	return brush;
}

Cell::Race Cell::GetRace()
{
	static const int DIFF = 50;
	byte r = GetRValue(brushColor);
	byte g = GetGValue(brushColor);
	byte b = GetBValue(brushColor);
	if (r - g >= DIFF && r - b >= DIFF)
		return Race::RED;
	if (g - r >= DIFF && g - b >= DIFF)
		return Race::GREEN;
	if (b - r >= DIFF && b - g >= DIFF)
		return Race::BLUE;
	return Race::NEUTRAL;
}

COLORREF Cell::GetBrushColor()
{
	return brushColor;
}

bool Cell::Equals(Cell* cell)
{
	return cell != nullptr && this->brushColor == cell->brushColor;
}

Cell* Cell::ProduceAvg(vector<Cell*> partners)
{
	int count = partners.size();
	int sumR = 0;
	int sumG = 0;
	int sumB = 0;
	for (int i = 0; i < count; ++i)
	{
		sumR += GetRValue(partners[i]->brushColor);
		sumG += GetGValue(partners[i]->brushColor);
		sumB += GetBValue(partners[i]->brushColor);
	}
	return new Cell(RGB(sumR / count, sumG / count, sumB / count));
}

Cell::~Cell()
{
	DeleteObject(this->brush);
}
