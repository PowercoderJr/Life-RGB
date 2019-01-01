#include "Cell.h"

Cell::Cell(byte r, byte g, byte b)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->brush = CreateSolidBrush(RGB(r, g, b));
}

HBRUSH Cell::GetBrush()
{
	return brush;
}

bool Cell::Equals(Cell * cell)
{
	return cell != nullptr && this->r == cell->r && this->g == cell->g && this->b == cell->b;
}

Cell::~Cell()
{
	DeleteObject(this->brush);
}
