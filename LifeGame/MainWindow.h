#pragma once
#include <Windows.h>
#include <commctrl.h>
#include "WorldWindow.h"

#define IDR_TOOLBAR						110
#define ID_CLEAR_GRID					40001
#define ID_GENERATE_GRID				40002
#define ID_SAVE_GRID					40003
#define ID_OPEN_GRID					40004
#define ID_SET_GRID_SIZE				40005
#define ID_SELECT_CELL_COLOR			40006
#define ID_SELECT_CELL_COLOR_RED		40007
#define ID_SELECT_CELL_COLOR_GREEN		40008
#define ID_SELECT_CELL_COLOR_BLUE		40009
#define ID_SELECT_CELL_COLOR_NEUTRAL	40010
#define ID_DRAW_MODE					40011
#define ID_PLAY_PAUSE					40012
#define ID_AUTOSTOP						40013
#define ID_DENSITY						40014
#define ID_SPEED						40015
#define ID_SPEED_x05					40016
#define ID_SPEED_x1						40017
#define ID_SPEED_x2						40018
#define ID_SPEED_x4						40019
#define ID_SPEED_x8						40020
#define ID_SPEED_x16					40021
#define ID_STATUS_BAR					40022

#define RED_RACE_ID 0
#define RED_RACE_PB racesPBs[RED_RACE_ID]
#define RED_RACE_LABEL racesLabels[RED_RACE_ID]
#define GREEN_RACE_ID 1
#define GREEN_RACE_PB racesPBs[GREEN_RACE_ID]
#define GREEN_RACE_LABEL racesLabels[GREEN_RACE_ID]
#define BLUE_RACE_ID 2
#define BLUE_RACE_PB racesPBs[BLUE_RACE_ID]
#define BLUE_RACE_LABEL racesLabels[BLUE_RACE_ID]
#define NEUTRAL_RACE_ID 4
#define NEUTRAL_RACE_PB racesPBs[NEUTRAL_RACE_ID]
#define NEUTRAL_RACE_LABEL racesLabels[NEUTRAL_RACE_ID]

/*
Dialog - ����� �����
MainMenu - ����, ����
PopupMenu - ���������, ���������, �������� ����, ������������� ����...
ToolBar - ������ ������
Button - ������ ������
RadioButton - �������� ���������� / ��������� ������� ��������
CheckBox - ���������� ����� ��������� ���� ����
ScrollBar - �� ������ �� ����������� ���������
StatusBar - ���������
ComboBox - �������� ���������� ����
ListBox - ������� / ������ / ����� / ����������� / ��������� / ���������
EditBox - ������ ����, ��������� ���������
*/

class MainWindow
{
public:
	MainWindow() {}
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, HINSTANCE);
	void Show();

protected:
	void OnCreate();
	void OnSize();
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
	HWND widthTB;
	HWND heightTB;
	HWND setGridSizeBtn;
	HWND colorPanel;
	HWND colorBtn;
	HWND colorLB;
	HWND drawLineRB;
	HWND drawAreaRB;
	HWND densityLabel;
	HWND densitySB;
	HWND autostopChB;
	HWND playPauseBtn;
	HWND speedCB;
	HWND racesPBs[4];
	HWND racesLabels[4];
	HWND statusBar;

	WorldWindow worldCanvas;

	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	void CreateMainMenu();
	void CreateToolbar();
	void CreateLeftPanel();
	void CreateGridWindow();
};
