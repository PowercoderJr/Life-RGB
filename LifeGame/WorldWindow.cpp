#include "WorldWindow.h"

WorldWindow::WorldWindow()
{	
	generation = 0;
	cells = NULL;
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
		that->lparam = lParam;
		that->OnLButtonDown();
		break;
	case WM_RBUTTONDOWN:
		that->lparam = lParam;
		that->OnRButtonDown();
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

void WorldWindow::OnCreate()
{
	SetTimer(this->handle, 1, 60, NULL);
	world.isPaused = true;
}

void WorldWindow::OnPaint()
{
	PAINTSTRUCT ps;
	RECT rc;
	GetClientRect(handle, &rc);

	HDC hDC = BeginPaint(handle, &ps);
	HDC cDC = CreateCompatibleDC(hDC);
	HBITMAP cBMP = CreateCompatibleBitmap(hDC, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

	HBITMAP hBMP = reinterpret_cast<HBITMAP>(SelectObject(cDC, cBMP));
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH oBrush = reinterpret_cast<HBRUSH>(SelectObject(cDC, hBrush));
	FillRect(cDC, &rc, hBrush);
	SelectObject(cDC, oBrush);
	DeleteObject(hBrush);

	this->world.DrawGrid(cDC, rc);
	this->world.DrawCells(cDC, rc);

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
	WorldWindow* ptr = this;
	RECT rec;
	GetClientRect(handle, &rec);

	if (!world.isPaused)
	{
		world.Update();
	}

	wchar_t str[200];
	_itow_s(world.generation, str, 10);
	std::wstring s1 = std::wstring(L"Generation: ");
	std::wstring s2 = std::wstring(str);
	SendMessageA(generation, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));
	_itow_s(world.cellsCounter, str, 10);
	s1 = std::wstring(L"Active cells: ");
	s2 = std::wstring(str);
	SendMessageA(cells, WM_SETTEXT, 0, reinterpret_cast<LPARAM>((s1 + s2).c_str()));

	InvalidateRect(handle, NULL, FALSE);
}

void WorldWindow::OnLButtonDown()
{
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);
	RECT rc;
	GetClientRect(handle, &rc);
	world.SetCell(rc, xPos, yPos, new Cell(0, 0, 0));
}

void WorldWindow::OnRButtonDown()
{
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);
	RECT rc;
	GetClientRect(handle, &rc);
	world.SetCell(rc, xPos, yPos, 0);
}

void WorldWindow::OnClose()
{
	DestroyWindow(this->handle);
}

void WorldWindow::OnDestroy()
{
	KillTimer(handle, 1);
	PostQuitMessage(WM_QUIT);
}