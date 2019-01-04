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
/*#define ID_SELECT_CELL_COLOR_RED		40008
#define ID_SELECT_CELL_COLOR_GREEN		40009
#define ID_SELECT_CELL_COLOR_BLUE		40010
#define ID_SELECT_CELL_COLOR_NEUTRAL	40011*/
#define ID_DRAW_MODE					40012
#define ID_PLAY_PAUSE					40013
#define ID_AUTOSTOP						40014
#define ID_DENSITY						40015
#define ID_SPEED						40016
#define ID_SPEED_x05					40017
#define ID_SPEED_x1						40018
#define ID_SPEED_x2						40019
#define ID_SPEED_x4						40020
#define ID_SPEED_x8						40021
#define ID_SPEED_x16					40022
#define ID_STATUS_BAR					40023
#define ID_WORLD_WINDOW					40024

/*#define RED_RACE_PB racesPBs[RED_RACE_ID]
#define RED_RACE_LABEL racesLabels[RED_RACE_ID]
#define GREEN_RACE_PB racesPBs[GREEN_RACE_ID]
#define GREEN_RACE_LABEL racesLabels[GREEN_RACE_ID]
#define BLUE_RACE_PB racesPBs[BLUE_RACE_ID]
#define BLUE_RACE_LABEL racesLabels[BLUE_RACE_ID]
#define NEUTRAL_RACE_PB racesPBs[NEUTRAL_RACE_ID]
#define NEUTRAL_RACE_LABEL racesLabels[NEUTRAL_RACE_ID]*/

/*
Dialog - выбор цвета
MainMenu - файл, поле
PopupMenu - сохранить, загрузить, очистить поле, сгенерировать поле...
ToolBar - всякие кнопки
Button - всякие кнопки
RadioButton - рисовать карандашом / заполнить область случайно
CheckBox - остановить когда останется одна раса
ScrollBar - на панели со статистикой населения
StatusBar - поколение
ComboBox - скорость обновления поля
ListBox - красные / зелёные / синие / нейтральные / случайные / кастомные
EditBox - размер поля, плотность населения
*/

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
	HWND speedCB;
	HWND racesPBs[RACES_COUNT];
	HWND racesLabels[RACES_COUNT];
	HWND statusBar;

	WorldWindow worldWindow;
	HANDLE lifeThread;
	HANDLE lifeMutex;
	bool isPaused;
	bool isClosing;
	bool isAutostopChecked;

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
	void OnWorldWindowClicked(WPARAM wParam, LPARAM lParam);

	void ClearWorld();
	void ClearWorld(int rowsCount, int colsCount);
	void ResetState();
	void DisplayStats();

	//void OnClicked(WPARAM wParam, LPARAM lParam);
};
