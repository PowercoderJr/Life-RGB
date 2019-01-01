#pragma once
#include "World.h"

class WorldWindow {
public:
	WorldWindow();
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, HINSTANCE, HWND, int, int, int, int);
	void Show();
	HWND handle;
	HWND generation;
	HWND cells;
	World world;
protected:
	void OnDestroy();
	void OnPaint();
	void OnTimer();
	void OnCreate();
	void OnClose();
	void OnLButtonDown();
	void OnRButtonDown();
private:
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
