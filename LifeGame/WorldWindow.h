#pragma once
#include "World.h"
#include <functional>

class WorldWindow {
public:
	WorldWindow();
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, int, int, int, int, HWND, HMENU, HINSTANCE);
	void Show();
	void ModifyArea(int, int, int, int, int, std::function<Cell*()> cellProvider);
	void ResetState();
	void SetBrushColor(COLORREF);
	COLORREF GetBrushColor();
	HBRUSH GetBrush();
	bool IsAreasMode();
	void SetIsAreasMode(bool isAreasMode);
	void SetIsAreaStartSelected(bool isAreaStartSelected);
	int GetAreaDensity();
	void SetAreaDensity(int areaDensity);
	void SetLifeMutex(HANDLE lifeMutex);
	HWND GetHandle();
	World* GetWorld();
	void SetWorld(World* world);
	~WorldWindow();
protected:
	void OnDestroy();
	void OnPaint();
	//void OnTimer();
	void OnCreate();
	void OnClose();
	void OnMouseButtonDown(UINT msg);
private:
	HWND handle;
	World* world;
	HANDLE lifeMutex;
	COLORREF brushColor;
	HBRUSH brush;
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	bool isAreasMode;
	int areaStartI;
	int areaStartJ;
	bool isAreaStartSelected;
	int areaDensity;
	void CoordsToIndices(const int x, const int y, int* i, int* j);
};
