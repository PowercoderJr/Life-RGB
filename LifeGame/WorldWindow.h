#pragma once
#include "World.h"

class WorldWindow {
public:
	WorldWindow();
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, HINSTANCE, HWND, int, int, int, int);
	void Show();
	void SetBrushColor(COLORREF);
	COLORREF GetBrushColor();
	bool IsAreasMode();
	void SetIsAreasMode(bool isAreasMode);
	void SetAreaDensity(int areaDensity);
	HWND GetHandle();
	World* GetWorld();
	void SetWorld(World* world);
	~WorldWindow();
protected:
	void OnDestroy();
	void OnPaint();
	void OnTimer();
	void OnCreate();
	void OnClose();
	void OnMouseButtonDown(UINT msg);
private:
	HWND handle;
	HWND generation;
	HWND cells;
	World* world;
	COLORREF brushColor;
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	bool isAreasMode;
	int areaStartI;
	int areaStartJ;
	bool isAreaStartSelected;
	int areaDensity;
	void CoordsToIndices(const int x, const int y, int* i, int* j);
};
