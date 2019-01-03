#include "MainWindow.h"

MainWindow::MainWindow()
{
	;
}

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

LRESULT __stdcall MainWindow::windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		MainWindow* that = reinterpret_cast<MainWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLongA(hwnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}
	MainWindow* that = reinterpret_cast<MainWindow*>(GetWindowLongA(hwnd, GWL_USERDATA));
	switch (message)
	{
	case WM_CREATE:
		that->handle = hwnd;
		that->OnCreate();
		return DefWindowProcA(hwnd, message, wParam, lParam);
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
	case WM_COMMAND:
		that->OnCommand(wParam, lParam);
		break;
	case WM_HSCROLL:
		that->OnDensitySbMoved(wParam, lParam);
		break;
	default:
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}
	return 0;
}

void MainWindow::OnCreate()
{
	CreateMainMenu();
	CreateToolbar();
	CreateLeftPanel();
	CreateWorldWindow();
}

void MainWindow::OnSize()
{
	static const int TOOLBAR_HEIGHT = 30;
	static const int STATUSBAR_HEIGHT = 22;
	static const int LEFT_PANEL_WIDTH = 250;
	RECT rect;
	GetClientRect(handle, &rect);
	SetWindowPos(leftPanel, HWND_TOP, 0, TOOLBAR_HEIGHT, LEFT_PANEL_WIDTH,
			rect.bottom - TOOLBAR_HEIGHT - STATUSBAR_HEIGHT, SWP_DRAWFRAME);
	SetWindowPos(worldWindow.GetHandle(), HWND_TOP, LEFT_PANEL_WIDTH,
			TOOLBAR_HEIGHT, rect.right - LEFT_PANEL_WIDTH,
			rect.bottom - TOOLBAR_HEIGHT - STATUSBAR_HEIGHT, SWP_DRAWFRAME);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	/*char values[1024];
	sprintf_s(values, "WPARAM: %ul, LPARAM: %ul\LO_WPARAM: %ul, LO_LPARAM: %ul\nHI_WPARAM: %ul, HI_LPARAM: %ul", wParam, lParam, LOWORD(wParam), LOWORD(lParam), HIWORD(wParam), HIWORD(lParam));
	MessageBoxExA(0, values, "Test", 0, 0);*/
	switch (LOWORD(wParam))
	{
	case ID_CLEAR_GRID:
		OnClearWorldClicked(wParam, lParam);
		break;
	case ID_SET_GRID_SIZE:
		OnSetGridSizeClicked(wParam, lParam);
		break;
	case ID_PLAY_PAUSE:
		OnPlayPauseClicked(wParam, lParam);
		break;
	case ID_SELECT_CELL_COLOR:
		OnSelectCellColorClicked(wParam, lParam);
		break;
	case ID_SELECT_CELL_COLOR_LISTBOX:
		OnSelectCellColorLbClicked(wParam, lParam);
		break;
	case ID_DRAW_MODE:
		OnDrawModeRbClicked(wParam, lParam);
		break;
	}
}

LRESULT MainWindow::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == colorPanel)
		return (LRESULT)worldWindow.GetBrushColor();
	return 0;
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
	tbButtons[0].idCommand = ID_CLEAR_GRID;
	tbButtons[0].fsState = TBSTATE_ENABLED;
	tbButtons[0].fsStyle = TBSTYLE_GROUP;
	tbButtons[1].iBitmap = STD_PROPERTIES;
	tbButtons[1].idCommand = ID_GENERATE_GRID;
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

	HWND label;
	label = CreateWindowExA(0, "STATIC", "Размер поля:", WS_CHILD | WS_VISIBLE,
			8, 40, 100, 20, handle, NULL, hInstance, NULL);
	rowsCountTB = CreateWindowExA(0, "EDIT", "100", WS_CHILD | WS_VISIBLE,
			106, 40, 30, 20, handle, NULL, hInstance, NULL);
	label = CreateWindowExA(0, "STATIC", "x", WS_CHILD | WS_VISIBLE,
			136, 40, 10, 20, handle, NULL, hInstance, NULL);
	colsCountTB = CreateWindowExA(0, "EDIT", "100", WS_CHILD | WS_VISIBLE,
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
			8, 100, 234, 100, handle, (HMENU)ID_SELECT_CELL_COLOR_LISTBOX, hInstance, NULL);
	SendMessageA(colorLB, LB_ADDSTRING, NULL, (LPARAM)"Красный");
	SendMessageA(colorLB, LB_ADDSTRING, NULL, (LPARAM)"Зелёный");
	SendMessageA(colorLB, LB_ADDSTRING, NULL, (LPARAM)"Синий");
	SendMessageA(colorLB, LB_ADDSTRING, NULL, (LPARAM)"Нейтральный");

	drawDotsRB = CreateWindowExA(0, "BUTTON", "Рисовать точечно", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			8, 210, 200, 20, handle, (HMENU)ID_DRAW_MODE, hInstance, NULL);
	drawAreasRB = CreateWindowExA(0, "BUTTON", "Рисовать области", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			8, 230, 200, 20, handle, (HMENU)ID_DRAW_MODE, hInstance, NULL);
	SendMessageA(drawDotsRB, BM_SETCHECK, BST_CHECKED, 0);

	densityLabel = CreateWindowExA(0, "STATIC", "Интенсивность: 100", WS_CHILD | WS_VISIBLE,
			8, 260, 234, 20, handle, NULL, hInstance, NULL);
	densitySB = CreateWindowExA(0, "SCROLLBAR", "", WS_CHILD | WS_VISIBLE,
			8, 280, 234, 20, handle, (HMENU)ID_DENSITY, hInstance, NULL);
	SetScrollRange(densitySB, SB_CTL, 1, 100, TRUE);
	SetScrollPos(densitySB, SB_CTL, 100, TRUE);
	EnableWindow(densityLabel, false);
	EnableWindow(densitySB, false);

	playPauseBtn = CreateWindowExA(0, "BUTTON", "Запустить симуляцию", WS_CHILD | WS_VISIBLE,
			8, 320, 234, 30, handle, (HMENU)ID_PLAY_PAUSE, hInstance, NULL);
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
	RED_RACE_PB = CreateWindowExA(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 480, 150, 20, handle, NULL, hInstance, NULL);
	RED_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 480, 76, 20, handle, NULL, hInstance, NULL);
	GREEN_RACE_PB = CreateWindowExA(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 510, 150, 20, handle, NULL, hInstance, NULL);
	GREEN_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 510, 76, 20, handle, NULL, hInstance, NULL);
	BLUE_RACE_PB = CreateWindowExA(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 540, 150, 20, handle, NULL, hInstance, NULL);
	BLUE_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 540, 76, 20, handle, NULL, hInstance, NULL);
	NEUTRAL_RACE_PB = CreateWindowExA(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, 570, 150, 20, handle, NULL, hInstance, NULL);
	NEUTRAL_RACE_LABEL = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			166, 570, 76, 20, handle, NULL, hInstance, NULL);
	SendMessageA(RED_RACE_PB, PBM_SETPOS, 10, 0);
	SendMessageA(GREEN_RACE_PB, PBM_SETPOS, 20, 0);
	SendMessageA(BLUE_RACE_PB, PBM_SETPOS, 30, 0);
	SendMessageA(NEUTRAL_RACE_PB, PBM_SETPOS, 40, 0);
	SendMessageA(RED_RACE_PB, PBM_SETBARCOLOR, 0, RED_COLOR_RGB);
	SendMessageA(GREEN_RACE_PB, PBM_SETBARCOLOR, 0, GREEN_COLOR_RGB);
	SendMessageA(BLUE_RACE_PB, PBM_SETBARCOLOR, 0, BLUE_COLOR_RGB);
	SendMessageA(NEUTRAL_RACE_PB, PBM_SETBARCOLOR, 0, NEUTRAL_COLOR_RGB);

	statusBar = CreateStatusWindowA(WS_CHILD | WS_VISIBLE, "", handle, ID_STATUS_BAR);
}

void MainWindow::CreateWorldWindow()
{
	worldWindow = WorldWindow();
	worldWindow.SetWorld(new World(100, 100));
	worldWindow.Register("WorldWindow", hInstance);
	worldWindow.Create("Grid", hInstance, handle, 250, 30, 500, 500);
}

void MainWindow::OnClearWorldClicked(WPARAM wParam, LPARAM lParam)
{
	int dialogResult = MessageBoxExA(handle,
		"Вы действительно хотите очистить поле?",
		"Подтвердите действие", MB_YESNO | MB_ICONQUESTION, NULL);
	if (dialogResult == IDYES)
	{
		int colsCount = worldWindow.GetWorld()->GetColsCount();
		int rowsCount = worldWindow.GetWorld()->GetRowsCount();
		worldWindow.SetWorld(new World(rowsCount, colsCount));
	}
}

void MainWindow::OnGenerateWorldClicked(WPARAM wParam, LPARAM lParam)
{
}

void MainWindow::OnOpenWorldClicked(WPARAM wParam, LPARAM lParam)
{
}

void MainWindow::OnSaveWorldClicked(WPARAM wParam, LPARAM lParam)
{
}

void MainWindow::OnSetGridSizeClicked(WPARAM wParam, LPARAM lParam)
{
	int colsCount, rowsCount;
	try
	{
		static const int ARGS_BUF_SIZE = 4;
		char buf[ARGS_BUF_SIZE];
		GetWindowTextA(rowsCountTB, buf, ARGS_BUF_SIZE);
		rowsCount = std::stoi(buf);
		GetWindowTextA(colsCountTB, buf, ARGS_BUF_SIZE);
		colsCount = std::stoi(buf);
	}
	catch (...)
	{
		rowsCount = -1;
		colsCount = -1;
	}

	if (rowsCount < 1 || rowsCount > 300 || colsCount < 1 || colsCount > 300)
	{
		MessageBoxExA(handle, "Укажите размер поля в пределах от 1х1 до 300x300",
			"Некорректный денные", MB_ICONWARNING, NULL);
	}
	else
	{
		int dialogResult = MessageBoxExA(handle,
			"Изменение размеров поля очистит его и обнулит текущую статистику. Продолжить?",
			"Подтвердите действие", MB_YESNO | MB_ICONQUESTION, NULL);
		if (dialogResult == IDYES)
		{
			worldWindow.SetWorld(new World(rowsCount, colsCount));
		}
	}
}

void MainWindow::OnDensitySbMoved(WPARAM wParam, LPARAM lParam)
{
	int pos = GetScrollPos(densitySB, SB_CTL);
	switch (LOWORD(wParam))
	{
	case SB_LINELEFT:
		pos -= 1;
		break;
	case SB_LINERIGHT:
		pos += 1;
		break;
	case SB_PAGELEFT:
		pos -= 10;
		break;
	case SB_PAGERIGHT:
		pos += 10;
		break;
	case SB_THUMBTRACK:
		pos = HIWORD(wParam);
		break;
	}
	SetScrollPos(densitySB, SB_CTL, pos, true);
	char buf[1024];
	sprintf_s(buf, "Интенсивность: %d", pos);
	SetWindowTextA(densityLabel, buf);
	worldWindow.SetAreaDensity(pos);
}

void MainWindow::OnPlayPauseClicked(WPARAM wParam, LPARAM lParam)
{
	worldWindow.GetWorld()->SetIsPaused(!worldWindow.GetWorld()->IsPaused());
	SetWindowTextA(playPauseBtn, worldWindow.GetWorld()->IsPaused() ?
		"Возобновить симуляцию" : "Приостановить симуляцию");
}

void MainWindow::OnSelectCellColorClicked(WPARAM wParam, LPARAM lParam)
{
	CHOOSECOLOR cc;
	COLORREF acrCustClr[16];
	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = handle;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = worldWindow.GetBrushColor();
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColorA(&cc))
		worldWindow.SetBrushColor(cc.rgbResult);
}

void MainWindow::OnSelectCellColorLbClicked(WPARAM wParam, LPARAM lParam)
{
	switch (HIWORD(wParam))
	{
	case LBN_SELCHANGE:
		int count = SendMessageA(colorLB, LB_GETCOUNT, 0, 0);
		int selectedIndex = -1;

		for (int i = 0; i < count && selectedIndex == -1; i++)
			if (SendMessageA(colorLB, LB_GETSEL, i, 0) > 0)
				selectedIndex = i;

		switch (selectedIndex)
		{
		case RED_RACE_ID:
			worldWindow.SetBrushColor(RED_COLOR_RGB);
			break;
		case GREEN_RACE_ID:
			worldWindow.SetBrushColor(GREEN_COLOR_RGB);
			break;
		case BLUE_RACE_ID:
			worldWindow.SetBrushColor(BLUE_COLOR_RGB);
			break;
		case NEUTRAL_RACE_ID:
			worldWindow.SetBrushColor(NEUTRAL_COLOR_RGB);
			break;
		default:
			break;
		}
		break;
	}
}

void MainWindow::OnDrawModeRbClicked(WPARAM wParam, LPARAM lParam)
{
	bool isAreasMode = (HWND)lParam == drawAreasRB;
	worldWindow.SetIsAreasMode(isAreasMode);
	SendMessageA(drawDotsRB, BM_SETCHECK, !isAreasMode, 0);
	SendMessageA(drawAreasRB, BM_SETCHECK, isAreasMode, 0);
	EnableWindow(densityLabel, isAreasMode);
	EnableWindow(densitySB, isAreasMode);
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
