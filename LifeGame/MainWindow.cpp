#include "MainWindow.h"
#include <time.h>
#include <functional>

using std::function;

const COLORREF colors[RACES_COUNT] = { RED_COLOR_RGB, GREEN_COLOR_RGB, BLUE_COLOR_RGB, NEUTRAL_COLOR_RGB };
const char colorNames[RACES_COUNT][32] = { "Красный", "Зелёный", "Синий", "Нейтральный" };

MainWindow::MainWindow()
{
	isPaused = true;
	isClosing = false;
	isAutostopChecked = false;
	speedFactor = 1;
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
	InvalidateRect(colorPanel, NULL, FALSE);

	lifeThread = CreateThread(NULL, NULL, LifeThreadFunction, this, CREATE_SUSPENDED, NULL);
	lifeMutex = CreateMutexA(NULL, FALSE, "lifeMutex");
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
	case ID_GENERATE_GRID:
		OnGenerateWorldClicked(wParam, lParam);
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
	case ID_AUTOSTOP:
		OnAutostopChbClicked(wParam, lParam);
		break;
	/*case ID_SPEED_FACTOR:
		OnNotify(wParam, lParam);
		break;*/
	case ID_SPEED_FACTOR:
		OnComboBoxNotify(wParam, lParam);
		break;
	case ID_WORLD_WINDOW:
		OnWorldWindowClicked(wParam, lParam);
		break;
	}
}

LRESULT MainWindow::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == colorPanel)
	{
		return (LRESULT)worldWindow.GetBrush();
	}
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
	for (int i = 0; i < RACES_COUNT; ++i)
		SendMessageA(colorLB, LB_ADDSTRING, NULL, (LPARAM)colorNames[i]);

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
	/*EnableWindow(densityLabel, false);
	EnableWindow(densitySB, false);*/

	playPauseBtn = CreateWindowExA(0, "BUTTON", "Запустить симуляцию", WS_CHILD | WS_VISIBLE,
			8, 320, 234, 30, handle, (HMENU)ID_PLAY_PAUSE, hInstance, NULL);
	autostopChB = CreateWindowExA(0, "BUTTON", "Остановить, если поле чистое", WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
			8, 360, 234, 20, handle, (HMENU)ID_AUTOSTOP, hInstance, NULL);
	label = CreateWindowExA(0, "STATIC", "Скорость симуляции:", WS_CHILD | WS_VISIBLE,
			8, 394, 150, 20, handle, NULL, hInstance, NULL);
	speedFactorCB = CreateWindowExA(0, "COMBOBOX", "", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | LBS_NOTIFY,
			166, 390, 76, 200, handle, (HMENU)ID_SPEED_FACTOR, hInstance, NULL);
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x0.25");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x0.5");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x1");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x2");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x4");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x8");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x16");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x32");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x64");
	SendMessageA(speedFactorCB, CB_ADDSTRING, NULL, (LPARAM)"x128");
	SendMessageA(speedFactorCB, CB_SETCURSEL, 2, 0);

	label = CreateWindowExA(0, "STATIC", "Статистика:", WS_CHILD | WS_VISIBLE,
			8, 450, 150, 20, handle, NULL, hInstance, NULL);

	for (int i = 0; i < RACES_COUNT; ++i)
	{
		int top = 480 + 30 * i;
		racesPBs[i] = CreateWindowExA(0, PROGRESS_CLASS, "", WS_CHILD | WS_VISIBLE,
			8, top, 134, 20, handle, NULL, hInstance, NULL);
		racesLabels[i] = CreateWindowExA(0, "STATIC", "", WS_CHILD | WS_VISIBLE,
			150, top, 92, 20, handle, NULL, hInstance, NULL);
		SendMessageA(racesPBs[i], PBM_SETBARCOLOR, 0, colors[i]);
	}

	statusBar = CreateStatusWindowA(WS_CHILD | WS_VISIBLE, "", handle, ID_STATUS_BAR);
}

void MainWindow::CreateWorldWindow()
{
	worldWindow = WorldWindow();
	worldWindow.SetLifeMutex(lifeMutex);
	WaitForSingleObject(lifeMutex, INFINITE);
	ClearWorld(100, 100);
	ReleaseMutex(lifeMutex);
	worldWindow.Register("WorldWindow", hInstance);
	worldWindow.Create("Grid", 250, 30, 500, 500, handle, (HMENU)ID_WORLD_WINDOW, hInstance);
}

void MainWindow::OnClearWorldClicked(WPARAM wParam, LPARAM lParam)
{
	int dialogResult = MessageBoxExA(handle,
		"Вы действительно хотите очистить поле?",
		"Подтвердите действие", MB_YESNO | MB_ICONQUESTION, NULL);
	if (dialogResult == IDYES)
	{
		WaitForSingleObject(lifeMutex, INFINITE);
		ClearWorld();
		ResetState();
		DisplayStats();
		ReleaseMutex(lifeMutex);
	}
}

void MainWindow::OnGenerateWorldClicked(WPARAM wParam, LPARAM lParam)
{
	int dialogResult = MessageBoxExA(handle,
		"Вы действительно хотите сгенерировать поле заново?",
		"Подтвердите действие", MB_YESNO | MB_ICONQUESTION, NULL);
	if (dialogResult == IDYES)
	{
		WaitForSingleObject(lifeMutex, INFINITE);
		ClearWorld();
		ResetState();
		function<Cell*()> cellProvider = []()->Cell*
		{
			return new Cell(RGB(rand() % 255, rand() % 255, rand() % 255));
		};
		worldWindow.ModifyArea(0, 0, worldWindow.GetWorld()->GetRowsCount() - 1,
				worldWindow.GetWorld()->GetColsCount() - 1,
				worldWindow.GetAreaDensity(), cellProvider);
		DisplayStats();
		InvalidateRect(worldWindow.GetHandle(), NULL, FALSE);
		ReleaseMutex(lifeMutex);
	}
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
			WaitForSingleObject(lifeMutex, INFINITE);
			ClearWorld(rowsCount, colsCount);
			ResetState();
			DisplayStats();
			ReleaseMutex(lifeMutex);
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

// TODO: если быстро два раза нажать кнопку, процесс виснет
void MainWindow::OnPlayPauseClicked(WPARAM wParam, LPARAM lParam)
{
	WaitForSingleObject(lifeMutex, INFINITE);
	if (isPaused)
	{
		ResumeThread(lifeThread);
		isPaused = false;
		SetWindowTextA(playPauseBtn, "Приостановить симуляцию");
	}
	else
	{
		SuspendThread(lifeThread);
		isPaused = true;
		SetWindowTextA(playPauseBtn, "Возобновить симуляцию");
	}
	ReleaseMutex(lifeMutex);
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
	{
		worldWindow.SetBrushColor(cc.rgbResult);
		InvalidateRect(colorPanel, NULL, FALSE);
	}
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
	InvalidateRect(colorPanel, NULL, FALSE);
}

void MainWindow::OnDrawModeRbClicked(WPARAM wParam, LPARAM lParam)
{
	bool isAreasMode = (HWND)lParam == drawAreasRB;
	worldWindow.SetIsAreasMode(isAreasMode);
	worldWindow.SetIsAreaStartSelected(false);
	SendMessageA(drawDotsRB, BM_SETCHECK, !isAreasMode, 0);
	SendMessageA(drawAreasRB, BM_SETCHECK, isAreasMode, 0);
	/*EnableWindow(densityLabel, isAreasMode);
	EnableWindow(densitySB, isAreasMode);*/
}

void MainWindow::OnAutostopChbClicked(WPARAM wParam, LPARAM lParam)
{
	isAutostopChecked = !isAutostopChecked;
	SendMessageA(autostopChB, BM_SETCHECK, isAutostopChecked, 0);
}

void MainWindow::OnComboBoxNotify(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
	case ID_SPEED_FACTOR:
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int index = SendMessageA(speedFactorCB, CB_GETCURSEL, 0, 0);
			if (index != CB_ERR)
			{
				char buf[32];
				SendMessageA(speedFactorCB, CB_GETLBTEXT, index, (LPARAM)&buf);
				speedFactor = std::atof(buf + 1);
			}
		}
		break;
	}
}

void MainWindow::OnWorldWindowClicked(WPARAM wParam, LPARAM lParam)
{
	DisplayStats();
}

void MainWindow::ClearWorld()
{
	int rowsCount = worldWindow.GetWorld()->GetRowsCount();
	int colsCount = worldWindow.GetWorld()->GetColsCount();
	ClearWorld(rowsCount, colsCount);
}

void MainWindow::ClearWorld(int rowsCount, int colsCount)
{
	worldWindow.SetWorld(new World(rowsCount, colsCount));
}

void MainWindow::ResetState()
{
	if (!isPaused)
	{
		isPaused = true;
		SuspendThread(lifeThread);
	}
	worldWindow.ResetState();
	SetWindowTextA(playPauseBtn, "Запустить симуляцию");
}

void MainWindow::DisplayStats()
{
	int total = worldWindow.GetWorld()->GetTotalCellsCount();
	if (total == 0)
	{
		for (int i = 0; i < RACES_COUNT; ++i)
		{
			SendMessageA(racesPBs[i], PBM_SETPOS, 0, 0);
			SetWindowTextA(racesLabels[i], "\0");
		}
	}
	else
	{
		for (int i = 0; i < RACES_COUNT; ++i)
		{
			int rc = worldWindow.GetWorld()->GetCellsCountByRace((Cell::Race)i);
			int percentage = rc * 100 / total;
			// TODO: на следующей строке процесс виснет, если закрыть программу во время обновления поля
			SendMessageA(racesPBs[i], PBM_SETPOS, percentage, 0);
			char buf[32];
			sprintf_s(buf, "%5d (%d%%)", rc, percentage);
			SetWindowTextA(racesLabels[i], buf);
		}
	}
}

DWORD MainWindow::LifeThreadFunction(LPVOID param)
{
	MainWindow* that = (MainWindow*)param; 
	while (!that->isClosing)
	{
		DWORD startMs = GetTickCount();
		WaitForSingleObject(that->lifeMutex, INFINITE);
		that->worldWindow.GetWorld()->Update();
		that->DisplayStats();
		ReleaseMutex(that->lifeMutex);
		InvalidateRect(that->worldWindow.GetHandle(), NULL, FALSE);
		if (that->worldWindow.GetWorld()->GetTotalCellsCount() == 0 &&
				that->isAutostopChecked)
		{
			SendMessageA(that->handle, WM_COMMAND, ID_PLAY_PAUSE, 0);
		}
		DWORD elapsedMs = GetTickCount() - startMs;
		DWORD delay = (DWORD)(LIFE_TICK_PERIOD_MS / that->speedFactor);
		if (elapsedMs < delay)
			Sleep(delay - elapsedMs);
	}
	return 0;
}

void MainWindow::OnClose()
{
	isClosing = true;
	if (isPaused)
		ResumeThread(lifeThread);
	WaitForSingleObject(lifeThread, 100/*00*/);
	CloseHandle(lifeThread);
	CloseHandle(lifeMutex);
	DestroyWindow(handle);
}

void MainWindow::OnDestroy()
{
	PostQuitMessage(WM_QUIT);
}
