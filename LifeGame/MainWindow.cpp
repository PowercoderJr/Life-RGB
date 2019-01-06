#include "MainWindow.h"
#include <time.h>
#include <functional>
#include <fileapi.h>
#include <shobjidl.h> 

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
	SetTooltips();
	InvalidateRect(colorPanel, NULL, FALSE);
	DisplayStats();

	lifeThread = CreateThread(NULL, NULL, LifeThreadFunction, this, NULL, NULL);
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
	SetWindowPos(statusBar, HWND_TOP, 0, rect.bottom - STATUSBAR_HEIGHT,
			rect.right, rect.bottom, SWP_DRAWFRAME);
	SendMessageA(toolbar, TB_AUTOSIZE, 0, 0);
}

void MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
	/*char values[1024];
	sprintf_s(values, "WPARAM: %ul, LPARAM: %ul\LO_WPARAM: %ul, LO_LPARAM: %ul\nHI_WPARAM: %ul, HI_LPARAM: %ul", wParam, lParam, LOWORD(wParam), LOWORD(lParam), HIWORD(wParam), HIWORD(lParam));
	MessageBoxExA(0, values, "Test", 0, 0);*/
	if ((HMENU)wParam == hMenuWorldClear || LOWORD(wParam) == ID_CLEAR_WORLD)
		OnClearWorldClicked(wParam, lParam);
	else if ((HMENU)wParam == hMenuWorldGenerate || LOWORD(wParam) == ID_GENERATE_WORLD)
		OnGenerateWorldClicked(wParam, lParam);
	else if ((HMENU)wParam == hMenuWorldLoad || LOWORD(wParam) == ID_OPEN_WORLD)
		OnOpenWorldClicked(wParam, lParam);
	else if ((HMENU)wParam == hMenuWorldSave || LOWORD(wParam) == ID_SAVE_WORLD)
		OnSaveWorldClicked(wParam, lParam);
	else
		switch (LOWORD(wParam))
		{
		case ID_SET_WORLD_SIZE:
			OnSetWorldSizeClicked(wParam, lParam);
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
	hMenuWorld = CreateMenu();
	AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hMenuWorld, "Мир");

	hMenuWorldSave = CreateMenu();
	hMenuWorldLoad = CreateMenu();
	hMenuWorldClear = CreateMenu();
	hMenuWorldGenerate = CreateMenu();
	AppendMenuA(hMenuWorld, MF_STRING, (UINT_PTR)hMenuWorldClear, "Очистить");
	AppendMenuA(hMenuWorldClear, MF_STRING, ID_CLEAR_WORLD, "Очистить");
	AppendMenuA(hMenuWorld, MF_STRING, (UINT_PTR)hMenuWorldGenerate, "Генерировать");
	AppendMenuA(hMenuWorldGenerate, MF_STRING, ID_GENERATE_WORLD, "Генерировать");
	AppendMenuA(hMenuWorld, MF_SEPARATOR, NULL, NULL);
	AppendMenuA(hMenuWorld, MF_STRING, (UINT_PTR)hMenuWorldSave, "Сохранить...");
	AppendMenuA(hMenuWorld, MF_STRING, (UINT_PTR)hMenuWorldLoad, "Загрузить...");

	DrawMenuBar(handle);
}

void MainWindow::CreateToolbar()
{
	tbButtons[0].iBitmap = STD_FILENEW;
	tbButtons[0].idCommand = ID_CLEAR_WORLD;
	tbButtons[0].fsState = TBSTATE_ENABLED;
	tbButtons[0].fsStyle = TBSTYLE_GROUP;
	tbButtons[1].iBitmap = STD_PROPERTIES;
	tbButtons[1].idCommand = ID_GENERATE_WORLD;
	tbButtons[1].fsState = TBSTATE_ENABLED;
	tbButtons[1].fsStyle = TBSTYLE_GROUP;
	tbButtons[2].fsStyle = TBSTYLE_SEP;
	tbButtons[3].iBitmap = STD_FILEOPEN;
	tbButtons[3].idCommand = ID_OPEN_WORLD;
	tbButtons[3].fsState = TBSTATE_ENABLED;
	tbButtons[3].fsStyle = TBSTYLE_GROUP;
	tbButtons[4].iBitmap = STD_FILESAVE;
	tbButtons[4].idCommand = ID_SAVE_WORLD;
	tbButtons[4].fsState = TBSTATE_ENABLED;
	tbButtons[4].fsStyle = TBSTYLE_GROUP;
	toolbar = CreateToolbarEx(handle, WS_CHILD | WS_VISIBLE | WS_DLGFRAME |
			TBSTYLE_TOOLTIPS, IDR_TOOLBAR, 0, HINST_COMMCTRL, IDB_STD_SMALL_COLOR,
			tbButtons, 5, 0, 0, 0, 0, sizeof(TBBUTTON));
	//SendMessageA(toolbar, TB_AUTOSIZE, 0, 0); // Отправляется из OnResize()
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
	setWorldSizeBtn = CreateWindowExA(0, "BUTTON", "OK", WS_CHILD | WS_VISIBLE,
			184, 40, 58, 20, handle, (HMENU)ID_SET_WORLD_SIZE, hInstance, NULL);
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
	int coords[] = { 200, 400, 600, -1 };
	SendMessageA(statusBar, SB_SETPARTS, 4, (LPARAM)coords);
}

void MainWindow::CreateWorldWindow()
{
	worldWindow = WorldWindow();
	worldWindow.SetLifeMutex(lifeMutex);
	WaitForSingleObject(lifeMutex, INFINITE);
	ClearWorld(100, 100);
	ReleaseMutex(lifeMutex);
	worldWindow.Register("WorldWindow", hInstance);
	worldWindow.Create("World", 250, 30, 500, 500, handle, (HMENU)ID_WORLD_WINDOW, hInstance);
}

void MainWindow::SetTooltips()
{
	SetTooltip(rowsCountTB, (char*)"Количество строк");
	SetTooltip(colsCountTB, (char*)"Количество столбцов");
	SetTooltip(drawDotsRB, (char*)"Один клик - одна клетка");
	SetTooltip(drawAreasRB, (char*)"Укажите противоположные углы прямоугольника");
	SetTooltip(autostopChB, (char*)"Приостановить симуляцию, если не останется ни одной живой клетки");
	SetTooltip(racesPBs[0], (char*)"Доля живущих красных клеток");
	SetTooltip(racesPBs[1], (char*)"Доля живущих зелёных клеток");
	SetTooltip(racesPBs[2], (char*)"Доля живущих синих клеток");
	SetTooltip(racesPBs[3], (char*)"Доля живущих нейтральных клеток");
}

void MainWindow::SetTooltip(HWND hwnd, char* tooltip)
{
	HWND hTooltip = CreateWindowExA(WS_EX_TOPMOST, TOOLTIPS_CLASS, "TooltipIM",
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		handle, NULL, hInstance, NULL);


	TOOLINFO ti;
	memset(&ti, 0, sizeof TOOLINFO);
	ti.cbSize = sizeof TOOLINFO;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.uId = (UINT)hwnd;
	ti.lpszText = tooltip;
	ti.hinst = hInstance;
	SendMessageA(hTooltip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

void MainWindow::ShowOpenFileDialog(char* filepath)
{
	char pathBuf[1024];
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						size_t x;
						wcstombs_s(&x, pathBuf, pszFilePath, wcslen(pszFilePath));
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	if (!SUCCEEDED(hr))
		sprintf_s(filepath, 1024, "\0");
	else
		sprintf_s(filepath, 1024, "%s", pathBuf);
}

void MainWindow::ShowSaveFileDialog(char* filepath)
{
	char pathBuf[1024];
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog *pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileSave->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr))
					{
						size_t x;
						wcstombs_s(&x, pathBuf, pszFilePath, wcslen(pszFilePath));
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
	if (!SUCCEEDED(hr))
		sprintf_s(filepath, 1024, "\0");
	else
		sprintf_s(filepath, 1024, "%s", pathBuf);
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
	char filepath[1024];
	ShowOpenFileDialog(filepath);
	if (strlen(filepath) > 0)
	{
		int dialogResult = MessageBoxExA(handle,
			"Вы действительно хотите открыть мир из файла? Текущее состояние будет утеряно.",
			"Подтвердите действие", MB_YESNO | MB_ICONQUESTION, NULL);
		if (dialogResult == IDYES)
		{
			ResetState();
			bool result = worldWindow.GetWorld()->ReadFromFile(filepath);
			InvalidateRect(worldWindow.GetHandle(), NULL, FALSE);
			DisplayStats();

			if (!result)
				MessageBoxExA(handle, "Во время чтения файла произошла ошибка", "Ошибка", MB_OK | MB_ICONERROR, NULL);
		}
	}
}

void MainWindow::OnSaveWorldClicked(WPARAM wParam, LPARAM lParam)
{
	char filepath[1024];
	ShowSaveFileDialog(filepath);
	if (strlen(filepath) > 0)
	{
		bool result = worldWindow.GetWorld()->WriteToFile(filepath);
		if (!result)
			MessageBoxExA(handle, "Во время записи файла произошла ошибка", "Ошибка", MB_OK | MB_ICONERROR, NULL);
	}
}

void MainWindow::OnSetWorldSizeClicked(WPARAM wParam, LPARAM lParam)
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

// TODO: если быстро несколько раз нажать кнопку, процесс виснет
void MainWindow::OnPlayPauseClicked(WPARAM wParam, LPARAM lParam)
{
	WaitForSingleObject(lifeMutex, INFINITE);
	if (isPaused)
	{
		isPaused = false;
		SetWindowTextA(playPauseBtn, "Приостановить симуляцию");
	}
	else
	{
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
	}
	worldWindow.ResetState();
	SetWindowTextA(playPauseBtn, "Запустить симуляцию");
}

void MainWindow::DisplayStats()
{
	char buf[64];
	int total = worldWindow.GetWorld()->GetTotalCellsCount();
	int square = worldWindow.GetWorld()->GetSquare();
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
			OutputDebugStringA("Before that line\n");
			SendMessageA(racesPBs[i], PBM_SETPOS, percentage, 0);
			OutputDebugStringA("After that line\n");
			sprintf_s(buf, "%5d (%d%%)", rc, percentage);
			SetWindowTextA(racesLabels[i], buf);
		}
	}

	sprintf_s(buf, "Поколение: %d\0", worldWindow.GetWorld()->GetGeneration());
	SendMessageA(statusBar, SB_SETTEXTA, MAKEWPARAM(LOBYTE(0), HIBYTE(SBT_POPOUT)), (LPARAM)buf);
	sprintf_s(buf, "Население: %5d/%-5d (%d%%)\0", total, square, square > 0 ? total * 100 / square : 0);
	SendMessageA(statusBar, SB_SETTEXTA, MAKEWPARAM(LOBYTE(1), HIBYTE(SBT_POPOUT)), (LPARAM)buf);
	sprintf_s(buf, "Клеток родилось: %d\0", worldWindow.GetWorld()->GetCellsBorn());
	SendMessageA(statusBar, SB_SETTEXTA, MAKEWPARAM(LOBYTE(2), HIBYTE(SBT_POPOUT)), (LPARAM)buf);
	sprintf_s(buf, "Клеток погибло: %d\0", worldWindow.GetWorld()->GetCellsDied());
	SendMessageA(statusBar, SB_SETTEXTA, MAKEWPARAM(LOBYTE(3), HIBYTE(SBT_POPOUT)), (LPARAM)buf);
}

DWORD MainWindow::LifeThreadFunction(LPVOID param)
{
	MainWindow* that = (MainWindow*)param; 
	while (!that->isClosing)
	{
		DWORD startMs = GetTickCount();
		if (!that->isPaused)
		{
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
	WaitForSingleObject(lifeThread, INFINITE);
	CloseHandle(lifeThread);
	CloseHandle(lifeMutex);
	DestroyWindow(handle);
}

void MainWindow::OnDestroy()
{
	PostQuitMessage(WM_QUIT);
}
