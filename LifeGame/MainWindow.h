#pragma once
#include <Windows.h>
#include <commctrl.h>
#include "WorldWindow.h"

#define IDR_TOOLBAR						110
#define ID_CLEAR_GRID					40001
#define ID_GENERATE_GRID				40002
#define ID_OPEN_GRID					40003
#define ID_SAVE_GRID					40004
#define ID_SET_GRID_SIZE				40005
#define ID_SELECT_CELL_COLOR			40006
#define ID_SELECT_CELL_COLOR_LISTBOX	40007
#define ID_DRAW_MODE					40012
#define ID_PLAY_PAUSE					40013
#define ID_AUTOSTOP						40014
#define ID_DENSITY						40015
#define ID_SPEED_FACTOR					40016
#define ID_STATUS_BAR					40023
#define ID_WORLD_WINDOW					40024

class MainWindow
{
public:
	static const int LIFE_TICK_PERIOD_MS = 1000;

	MainWindow();
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, HINSTANCE);
	void Show();

protected:
	void OnCreate();
	void OnSize();
	void OnCommand(WPARAM, LPARAM);
	LRESULT OnCtlColorStatic(WPARAM, LPARAM);
	void OnClose();
	void OnDestroy();

private:
	HINSTANCE hInstance;
	HWND handle;
	LPARAM lparam;

	HMENU hMenu;
	HMENU hMenuGrid;
	HMENU hMenuGridSave;
	HMENU hMenuGridLoad;
	HMENU hMenuGridClear;
	HMENU hMenuGridGenerate;

	HWND hToolbar;
	TBBUTTON tbButtons[5];

	HWND leftPanel;
	HWND rowsCountTB;
	HWND colsCountTB;
	HWND setGridSizeBtn;
	HWND colorPanel;
	HWND colorBtn;
	HWND colorLB;
	HWND drawDotsRB;
	HWND drawAreasRB;
	HWND densityLabel;
	HWND densitySB;
	HWND autostopChB;
	HWND playPauseBtn;
	HWND speedFactorCB;
	HWND racesPBs[RACES_COUNT];
	HWND racesLabels[RACES_COUNT];
	HWND statusBar;

	WorldWindow worldWindow;
	HANDLE lifeThread;
	HANDLE lifeMutex;
	bool isPaused;
	bool isClosing;
	bool isAutostopChecked;
	float speedFactor;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI LifeThreadFunction(LPVOID param);
	void CreateMainMenu();
	void CreateToolbar();
	void CreateLeftPanel();
	void CreateWorldWindow();

	void OnClearWorldClicked(WPARAM wParam, LPARAM lParam);
	void OnGenerateWorldClicked(WPARAM wParam, LPARAM lParam);
	void OnOpenWorldClicked(WPARAM wParam, LPARAM lParam);
	void OnSaveWorldClicked(WPARAM wParam, LPARAM lParam);
	void OnSetGridSizeClicked(WPARAM wParam, LPARAM lParam);
	void OnDensitySbMoved(WPARAM wParam, LPARAM lParam);
	void OnPlayPauseClicked(WPARAM wParam, LPARAM lParam);
	void OnSelectCellColorClicked(WPARAM wParam, LPARAM lParam);
	void OnSelectCellColorLbClicked(WPARAM wParam, LPARAM lParam);
	void OnDrawModeRbClicked(WPARAM wParam, LPARAM lParam);
	void OnAutostopChbClicked(WPARAM wParam, LPARAM lParam);
	void OnComboBoxNotify(WPARAM wParam, LPARAM lParam);
	void OnWorldWindowClicked(WPARAM wParam, LPARAM lParam);

	void ClearWorld();
	void ClearWorld(int rowsCount, int colsCount);
	void ResetState();
	void DisplayStats();

	//void OnClicked(WPARAM wParam, LPARAM lParam);
};
