#include "WorldWindow.h"
#include <stdlib.h>

using std::function;

WorldWindow::WorldWindow()
{	
	world = nullptr;
	isAreasMode = false;
	areaStartI = 0;
	areaStartJ = 0;
	isAreaStartSelected = false;
	areaDensity = 100;
	SetBrushColor(0);
}

bool WorldWindow::Register(const char* name, HINSTANCE hInstance)
{
	WNDCLASSEX tag;
	tag.cbSize = sizeof(WNDCLASSEX);
	tag.style = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc = windowProc;
	tag.cbClsExtra = 0;
	tag.cbWndExtra = 0;
	tag.hIcon = LoadIconA(NULL, IDI_APPLICATION);
	tag.hCursor = LoadCursorA(NULL, IDC_ARROW);
	tag.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	tag.lpszMenuName = NULL;
	tag.lpszClassName = "WorldWindow";
	tag.hInstance = hInstance;
	tag.hIconSm = LoadIconA(NULL, IDI_APPLICATION);
	return RegisterClassExA(&tag) != 0;
}

bool WorldWindow::Create(const char* name, int x, int y, int w, int h, HWND hwnd, HMENU hMenu, HINSTANCE hInstance)
{
	handle = CreateWindowExA(0, "WorldWindow", name, WS_CHILD | WS_BORDER,
			x, y, w, h, hwnd, hMenu, hInstance, this);
	this->Show();
	return true;
}

void WorldWindow::Show()
{
	ShowWindow(handle, SW_SHOWNORMAL);
	UpdateWindow(handle);
}

void WorldWindow::ModifyArea(int i0, int j0, int i1, int j1, int density, function<Cell*()> cellProvider)
{
	for (int iRun = i0; iRun <= i1; ++iRun)
		for (int jRun = j0; jRun <= j1; ++jRun)
			if (rand() % 100 < density)
				world->SetCell(iRun, jRun, cellProvider());
}

void WorldWindow::ResetState()
{
	world->ResetState();
	areaStartI = 0;
	areaStartJ = 0;
	isAreaStartSelected = false;
	InvalidateRect(handle, NULL, FALSE);
}

void WorldWindow::SetBrushColor(COLORREF brushColor)
{
	this->brushColor = brushColor;
	DeleteObject(brush);
	brush = CreateSolidBrush(this->brushColor);
}

COLORREF WorldWindow::GetBrushColor()
{
	return brushColor;
}

HBRUSH WorldWindow::GetBrush()
{
	return brush;
}

bool WorldWindow::IsAreasMode()
{
	return isAreasMode;
}

void WorldWindow::SetIsAreasMode(bool isAreasMode)
{
	this->isAreasMode = isAreasMode;
}

void WorldWindow::SetIsAreaStartSelected(bool isAreaStartSelected)
{
	this->isAreaStartSelected = isAreaStartSelected;
}

int WorldWindow::GetAreaDensity()
{
	return areaDensity;
}

void WorldWindow::SetAreaDensity(int areaDensity)
{
	this->areaDensity = areaDensity;
}

void WorldWindow::SetLifeMutex(HANDLE lifeMutex)
{
	this->lifeMutex = lifeMutex;
}

HWND WorldWindow::GetHandle()
{
	return handle;
}

World * WorldWindow::GetWorld()
{
	return world;
}

void WorldWindow::SetWorld(World * world)
{
	if (this->world != nullptr)
		delete this->world;
	this->world = world;
}

LRESULT __stdcall WorldWindow::windowProc(HWND hnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		WorldWindow* that = reinterpret_cast<WorldWindow*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		SetWindowLong(hnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return DefWindowProcA(hnd, message, wParam, lParam);
	}
	WorldWindow* that = reinterpret_cast<WorldWindow*>(GetWindowLongA(hnd, GWL_USERDATA));
	switch (message)
	{
	case WM_CREATE:
		that->handle = hnd;
		that->OnCreate();
		return DefWindowProcA(hnd, message, wParam, lParam);
	case WM_PAINT:
		that->OnPaint();
		break;
	case WM_TIMER:
		//that->OnTimer();
		break;
	case WM_DESTROY:
		that->OnDestroy();
		break;
	case WM_CLOSE:
		that->OnClose();
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		that->lparam = lParam;
		that->OnMouseButtonDown(message);
		break;
	default:
		return DefWindowProcA(hnd, message, wParam, lParam);
	}
	return 0;
}

void WorldWindow::CoordsToIndices(const int x, const int y, int* i, int* j)
{
	RECT rc;
	GetClientRect(handle, &rc);
	*i = y * world->GetRowsCount() / (rc.bottom - rc.top);
	*j = x * world->GetColsCount() / (rc.right - rc.left);
}

void WorldWindow::OnCreate()
{
	SetTimer(this->handle, 1, 60, NULL);
}

void WorldWindow::OnPaint()
{
	PAINTSTRUCT ps;
	RECT rc;
	GetClientRect(handle, &rc);

	// TODO: иногда возвращает NULL
	HDC hDC = BeginPaint(handle, &ps);
	HDC cDC = CreateCompatibleDC(hDC);
	HBITMAP cBMP = CreateCompatibleBitmap(hDC, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

	HBITMAP hBMP = reinterpret_cast<HBITMAP>(SelectObject(cDC, cBMP));
	HBRUSH hBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(100, 100, 100));
	HBRUSH oBrush = reinterpret_cast<HBRUSH>(SelectObject(cDC, hBrush));
	FillRect(cDC, &rc, hBrush);
	SelectObject(cDC, oBrush);
	DeleteObject(hBrush);

	this->world->DrawGrid(cDC, rc);
	this->world->DrawCells(cDC, rc);

	BitBlt(hDC,
		ps.rcPaint.left,
		ps.rcPaint.top,
		ps.rcPaint.right - ps.rcPaint.left,
		ps.rcPaint.bottom - ps.rcPaint.top,
		cDC,
		ps.rcPaint.left,
		ps.rcPaint.top,
		SRCCOPY);

	SelectObject(cDC, hBMP);
	DeleteObject(cBMP);
	DeleteDC(cDC);
	EndPaint(handle, &ps);
}

/*void WorldWindow::OnTimer()
{
	/*WorldWindow* ptr = this;
	RECT rect;
	GetClientRect(handle, &rect);*
	if (!world->IsPaused())
		world->Update();

	/*wchar_t str[200];
	_itow_s(world.generation, str, 10);
	std::wstring s1 = std::wstring(L"Generation: ");
	std::wstring s2 = std::wstring(str);
	SendMessageA(generation, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));
	_itow_s(world.totalCellsCount, str, 10);
	s1 = std::wstring(L"Active cells: ");
	s2 = std::wstring(str);
	SendMessageA(cells, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));*

	InvalidateRect(handle, NULL, FALSE);
}*/

void WorldWindow::OnMouseButtonDown(UINT msg)
{
	WaitForSingleObject(lifeMutex, INFINITE);
	bool isLMB = msg == WM_LBUTTONDOWN;
	int i, j;
	CoordsToIndices(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), &i, &j);
	if (isAreasMode)
	{
		if (isAreaStartSelected)
		{
			int i0 = min(areaStartI, i);
			int j0 = min(areaStartJ, j);
			int i1 = max(areaStartI, i);
			int j1 = max(areaStartJ, j);

			function<Cell*()> cellProvider = [this, isLMB]()->Cell*
			{
				if (isLMB)
					return new Cell(brushColor);
				else
					return nullptr;
			};
			ModifyArea(i0, j0, i1, j1, isLMB ? areaDensity : 100, cellProvider);
		}
		else
		{
			areaStartI = i;
			areaStartJ = j;
		}
		isAreaStartSelected = !isAreaStartSelected;
	}
	else
	{
		world->SetCell(i, j, isLMB ? new Cell(brushColor) : nullptr);
	}
	InvalidateRect(handle, NULL, FALSE);
	SendMessageA(GetParent(handle), WM_COMMAND, (WPARAM)GetMenu(handle), lparam);
	ReleaseMutex(lifeMutex);
}

void WorldWindow::OnClose()
{
	DestroyWindow(this->handle);
}

WorldWindow::~WorldWindow()
{
	DeleteObject(brush);
	delete world;
}

void WorldWindow::OnDestroy()
{
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}