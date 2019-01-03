#include "WorldWindow.h"
#include <stdlib.h>
#include <time.h>

WorldWindow::WorldWindow()
{	
	generation = 0;
	cells = NULL;
	brushColor = 0;
	world = nullptr;
	isAreasMode = false;
	areaStartI = 0;
	areaStartJ = 0;
	isAreaStartSelected = false;
	areaDensity = 100;
	srand(time(NULL));
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

bool WorldWindow::Create(const char* name, HINSTANCE hInstance, HWND hnd, int x, int y, int w, int h)
{
	handle = CreateWindowExA(0, "WorldWindow", name, WS_CHILD | WS_BORDER,
			x, y, w, h, hnd, 0, hInstance, this);
	this->Show();
	return true;
}

void WorldWindow::Show()
{
	ShowWindow(handle, SW_SHOWNORMAL);
	UpdateWindow(handle);
}

void WorldWindow::SetBrushColor(COLORREF brushColor)
{
	this->brushColor = brushColor;
}

COLORREF WorldWindow::GetBrushColor()
{
	return brushColor;
}

bool WorldWindow::IsAreasMode()
{
	return isAreasMode;
}

void WorldWindow::SetIsAreasMode(bool isAreasMode)
{
	this->isAreasMode = isAreasMode;
}

void WorldWindow::SetAreaDensity(int areaDensity)
{
	this->areaDensity = areaDensity;
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
		that->OnTimer();
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
	/*case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_PAUSE:
			that->world.paused = !that->world.paused;
			break;
		}*/
	default:
		return DefWindowProcA(hnd, message, wParam, lParam);
	}
	return 0;
}

void WorldWindow::PixelsToCells(int * x_to_i, int * y_to_j)
{
	RECT rc;
	GetClientRect(handle, &rc);
	// TODO?: int <-> float
	*x_to_i = *x_to_i * world->GetWidth() / (rc.right - rc.left);
	*y_to_j = *y_to_j * world->GetHeight() / (rc.bottom - rc.top);
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

void WorldWindow::OnTimer()
{
	/*WorldWindow* ptr = this;
	RECT rect;
	GetClientRect(handle, &rect);*/
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
	SendMessageA(cells, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));*/

	InvalidateRect(handle, NULL, FALSE);
}

void WorldWindow::OnMouseButtonDown(UINT msg)
{
	bool isLMB = msg == WM_LBUTTONDOWN;
	int i = GET_X_LPARAM(lparam);
	int j = GET_Y_LPARAM(lparam);
	PixelsToCells(&i, &j);
	if (isAreasMode)
	{
		if (isAreaStartSelected)
		{
			int i0 = min(areaStartI, i);
			int j0 = min(areaStartJ, j);
			int i1 = max(areaStartI, i);
			int j1 = max(areaStartJ, j);
			for (int iRun = i0; iRun <= i1; ++iRun)
				for (int jRun = j0; jRun <= j1; ++jRun)
					if (!isLMB || rand() % 100 < areaDensity)
						world->SetCell(iRun, jRun, isLMB ? new Cell(brushColor) : nullptr);
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
}

void WorldWindow::OnClose()
{
	DestroyWindow(this->handle);
}

WorldWindow::~WorldWindow()
{
	delete world;
}

void WorldWindow::OnDestroy()
{
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}