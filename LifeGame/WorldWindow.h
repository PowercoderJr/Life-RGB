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
	HWND handle;
	HWND generation;
	HWND cells;
	World* world;
	~WorldWindow();
protected:
	void OnDestroy();
	void OnPaint();
	void OnTimer();
	void OnCreate();
	void OnClose();
	void OnLButtonDown();
	void OnRButtonDown();
private:
	COLORREF brushColor;
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
