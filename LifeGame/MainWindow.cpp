#include "MainWindow.h"

bool MainWindow::Register(const char* name, HINSTANCE hInstance)
{
	WNDCLASSEX tag = {};
	tag.cbSize = sizeof(WNDCLASSEX);
	tag.style = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc = windowProc;
	tag.cbClsExtra = 0;
	tag.cbWndExtra = 0;
	//tag.hIcon = static_cast<HICON>(LoadImageA(hInstance, MAKEINTRESOURCEA(IDI_ICON2), 1, 32, 32, NULL));
	tag.hCursor = LoadCursorA(NULL, IDC_ARROW);
	tag.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	tag.lpszClassName = "MainWindow";
	tag.hInstance = hInstance;
	tag.hIconSm = 0;
	return RegisterClassExA(&tag) != 0;
}

bool MainWindow::Create(const char* name, HINSTANCE hInstance)
{
	hMenu = CreateMenu();
	this->hInstance = hInstance;
	handle = CreateWindowExA(0, "MainWindow", name, WS_OVERLAPPEDWINDOW, 50, 50, 1000, 800, 0, hMenu, hInstance, this);
	return true;
}

void MainWindow::Show()
{
	ShowWindow(handle, SW_SHOWNORMAL);
	UpdateWindow(handle);
}

LRESULT __stdcall MainWindow::windowProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		MainWindow* that = reinterpret_cast<MainWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLongA(hnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return DefWindowProcA(hnd, message, wParam, lParam);
	}
	MainWindow* that = reinterpret_cast<MainWindow*>(GetWindowLongA(hnd, GWL_USERDATA));
	switch (message)
	{
	case WM_CREATE:
		that->handle = hnd;
		that->OnCreate();
		return DefWindowProcA(hnd, message, wParam, lParam);
	case WM_SIZE:
		that->lparam = lParam;
		that->OnSize();
		break;
	case WM_CTLCOLORSTATIC:
		return that->OnCtlColorStatic(wParam, lParam);
	case WM_CLOSE:
		that->OnClose();
		break;
	case WM_DESTROY:
		that->OnDestroy();
		break;
	/*case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_NEW:
			that->NewWorld();
			break;
		case ID_LOAD:
			that->LoadWorld();
			break;
		case ID_EXIT:
			that->OnClose();
			break;
		}
		break;*/
	default:
		return DefWindowProcA(hnd, message, wParam, lParam);
	}
	return 0;
}

void MainWindow::OnCreate()
{
	CreateMainMenu();
	CreateToolbar();
	CreateLeftPanel();
	OnSize();
}

void MainWindow::OnSize()
{
	static const int TOOLBAR_HEIGHT = 30;
	static const int LEFT_PANEL_WIDTH = 250;
	RECT rec;
	GetClientRect(handle, &rec);
	SetWindowPos(leftPanel, HWND_TOP, 0, TOOLBAR_HEIGHT, LEFT_PANEL_WIDTH, rec.bottom, SWP_DRAWFRAME);
	/*SetWindowPos(hwnd_gen_text, HWND_TOP, rec.right*0.8 + 10, 20, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(hwnd_cells_text, HWND_TOP, rec.right*0.8 + 10, 60, rec.right*0.2 - 20, 20, SWP_DRAWFRAME);
	SetWindowPos(hwnd_help_text, HWND_TOP, rec.right*0.8 + 10, 150, rec.right*0.2 - 20, 80, SWP_DRAWFRAME);
	SetWindowPos(hwnd_size_text, HWND_TOP, rec.right*0.8 + 10, 100, 50, 20, SWP_DRAWFRAME);
	SetWindowPos(hwnd_size_edit, HWND_TOP, rec.right*0.8 + 65, 100, rec.right*0.2 - 20, 80, SWP_DRAWFRAME);*/
}

LRESULT MainWindow::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == colorPanel)
	{
		HDC hdcStatic = HDC(wParam);
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, RGB(230, 230, 230));
		return (LRESULT)CreateSolidBrush(RGB(255, 0, 0));
	}
}

void MainWindow::CreateMainMenu()
{
	hMenuGrid = CreateMenu();
	AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hMenuGrid, "Поле");

	hMenuGridSave = CreateMenu();
	hMenuGridLoad = CreateMenu();
	hMenuGridClear = CreateMenu();
	hMenuGridGenerate = CreateMenu();
	AppendMenuA(hMenuGrid, MF_STRING, (UINT_PTR)hMenuGridClear, "Очистить");
	AppendMenuA(hMenuGrid, MF_STRING, (UINT_PTR)hMenuGridGenerate, "Генерировать");
	AppendMenuA(hMenuGrid, MF_SEPARATOR, NULL, NULL);
	AppendMenuA(hMenuGrid, MF_STRING, (UINT_PTR)hMenuGridSave, "Сохранить...");
	AppendMenuA(hMenuGrid, MF_STRING, (UINT_PTR)hMenuGridLoad, "Загрузить...");

	DrawMenuBar(handle);
}

void MainWindow::CreateToolbar()
{
	tbButtons[0].iBitmap = STD_FILENEW;
	tbButtons[0].idCommand = ID_OPEN_GRID;
	tbButtons[0].fsState = TBSTATE_ENABLED;
	tbButtons[0].fsStyle = TBSTYLE_GROUP;
	tbButtons[1].iBitmap = STD_PROPERTIES;
	tbButtons[1].idCommand = ID_SAVE_GRID;
	tbButtons[1].fsState = TBSTATE_ENABLED;
	tbButtons[1].fsStyle = TBSTYLE_GROUP;
	tbButtons[2].fsStyle = TBSTYLE_SEP;
	tbButtons[3].iBitmap = STD_FILEOPEN;
	tbButtons[3].idCommand = ID_OPEN_GRID;
	tbButtons[3].fsState = TBSTATE_ENABLED;
	tbButtons[3].fsStyle = TBSTYLE_GROUP;
	tbButtons[4].iBitmap = STD_FILESAVE;
	tbButtons[4].idCommand = ID_SAVE_GRID;
	tbButtons[4].fsState = TBSTATE_ENABLED;
	tbButtons[4].fsStyle = TBSTYLE_GROUP;
	hToolbar = CreateToolbarEx(handle, WS_CHILD | WS_VISIBLE | WS_DLGFRAME |
			TBSTYLE_TOOLTIPS, IDR_TOOLBAR, 0, HINST_COMMCTRL, IDB_STD_SMALL_COLOR,
			tbButtons, 5, 0, 0, 0, 0, sizeof(TBBUTTON));
}

void MainWindow::CreateLeftPanel()
{
	leftPanel = CreateWindowExA(0, "STATIC", NULL, WS_CHILD | WS_VISIBLE,
			0, 0, 300, 300, handle, NULL, hInstance, NULL);
	/*
	HWND statusBar;*/
	HWND label;
	label = CreateWindowExA(0, "STATIC", "Размер поля:", WS_CHILD | WS_VISIBLE,
			8, 40, 100, 20, handle, NULL, hInstance, NULL);
	widthTB = CreateWindowExA(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE,
			106, 40, 30, 20, handle, NULL, hInstance, NULL);
	label = CreateWindowExA(0, "STATIC", "x", WS_CHILD | WS_VISIBLE,
			136, 40, 10, 20, handle, NULL, hInstance, NULL);
	heightTB = CreateWindowExA(0, "EDIT", NULL, WS_CHILD | WS_VISIBLE,
			146, 40, 30, 20, handle, NULL, hInstance, NULL);
	setGridSizeBtn = CreateWindowExA(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE,
			184, 40, 58, 20, handle, (HMENU)ID_SET_GRID_SIZE, hInstance, NULL);
	label = CreateWindowExA(0, "STATIC", "Цвет клеток:", WS_CHILD | WS_VISIBLE,
			8, 70, 100, 20, handle, NULL, hInstance, NULL);
	colorPanel = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			106, 70, 48, 20, handle, NULL, hInstance, NULL);
	colorBtn = CreateWindowExA(0, "BUTTON", "Выбрать...", WS_CHILD | WS_VISIBLE,
			162, 70, 80, 20, handle, (HMENU)ID_SELECT_CELL_COLOR, hInstance, NULL);
	colorLB = CreateWindowExA(0, "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD &~ LBS_SORT | LBS_NOTIFY, 
			8, 100, 234, 100, handle, NULL, hInstance, NULL);
	SendMessageA(colorLB, LB_ADDSTRING, ID_SELECT_CELL_COLOR_RED, (LPARAM)"Красный");
	SendMessageA(colorLB, LB_ADDSTRING, ID_SELECT_CELL_COLOR_GREEN, (LPARAM)"Зелёный");
	SendMessageA(colorLB, LB_ADDSTRING, ID_SELECT_CELL_COLOR_BLUE, (LPARAM)"Синий");
	SendMessageA(colorLB, LB_ADDSTRING, ID_SELECT_CELL_COLOR_NEUTRAL, (LPARAM)"Нейтральный");

	// TODO: не получается изменить значение во время выполнения
	densityLabel = CreateWindowExA(0, "STATIC", "Интенсивность: 100", WS_CHILD | WS_VISIBLE,
			8, 210, 234, 20, handle, NULL, hInstance, NULL);
	densitySB = CreateWindowExA(0, "SCROLLBAR", "", WS_CHILD | WS_VISIBLE,
			8, 230, 234, 20, handle, (HMENU)ID_DENSITY, hInstance, NULL);
	SetScrollRange(densitySB, SB_CTL, 1, 100, TRUE);
	SetScrollPos(densitySB, SB_CTL, 100, TRUE);

	drawLineRB = CreateWindowExA(0, "BUTTON", "Рисовать линии", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			8, 260, 200, 20, handle, (HMENU)ID_DRAW_MODE, hInstance, NULL);
	drawAreaRB = CreateWindowExA(0, "BUTTON", "Рисовать области", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			8, 280, 200, 20, handle, (HMENU)ID_DRAW_MODE, hInstance, NULL);
	playPauseBtn = CreateWindowExA(0, "BUTTON", "Запустить симуляцию", WS_CHILD | WS_VISIBLE,
			8, 320, 234, 30, handle, (HMENU)ID_SELECT_CELL_COLOR, hInstance, NULL);
	autostopChB = CreateWindowExA(0, "BUTTON", "Остановить, если поле чистое", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			8, 360, 234, 20, handle, (HMENU)ID_AUTOSTOP, hInstance, NULL);
	label = CreateWindowExA(0, "STATIC", "Скорость симуляции:", WS_CHILD | WS_VISIBLE,
			8, 394, 150, 20, handle, NULL, hInstance, NULL);
	speedCB = CreateWindowExA(0, "COMBOBOX", "", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,
			166, 390, 76, 200, handle, (HMENU)ID_SPEED, hInstance, NULL);
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x05, (LPARAM)"x0.5");
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x1, (LPARAM)"x1");
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x2, (LPARAM)"x2");
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x4, (LPARAM)"x4");
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x8, (LPARAM)"x8");
	SendMessageA(speedCB, CB_ADDSTRING, ID_SPEED_x16, (LPARAM)"x16");
	SendMessageA(speedCB, CB_SETCURSEL, 1, 0);

	label = CreateWindowExA(0, "STATIC", "Статистика:", WS_CHILD | WS_VISIBLE,
			8, 450, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_PB = CreateWindowEx(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 480, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 480, 76, 20, handle, NULL, hInstance, NULL);
	GREEN_RACE_PB = CreateWindowEx(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 510, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 510, 76, 20, handle, NULL, hInstance, NULL);
	BLUE_RACE_PB = CreateWindowEx(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 540, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 540, 76, 20, handle, NULL, hInstance, NULL);
	NEUTRAL_RACE_PB = CreateWindowEx(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 570, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 570, 76, 20, handle, NULL, hInstance, NULL);
	SendMessageA(RED_RACE_PB, PBM_SETPOS, 10, 0);
	SendMessageA(GREEN_RACE_PB, PBM_SETPOS, 20, 0);
	SendMessageA(BLUE_RACE_PB, PBM_SETPOS, 30, 0);
	SendMessageA(NEUTRAL_RACE_PB, PBM_SETPOS, 40, 0);
	SendMessageA(RED_RACE_PB, PBM_SETBARCOLOR, 0, RGB(255, 0, 0));
	SendMessageA(GREEN_RACE_PB, PBM_SETBARCOLOR, 0, RGB(0, 255, 0));
	SendMessageA(BLUE_RACE_PB, PBM_SETBARCOLOR, 0, RGB(0, 0, 255));
	SendMessageA(NEUTRAL_RACE_PB, PBM_SETBARCOLOR, 0, RGB(150, 150, 150));

	statusBar = CreateStatusWindowA(WS_CHILD | WS_VISIBLE, "", handle, ID_STATUS_BAR);
}

void MainWindow::OnClose()
{
	DestroyWindow(handle);
}

void MainWindow::OnDestroy()
{
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}
