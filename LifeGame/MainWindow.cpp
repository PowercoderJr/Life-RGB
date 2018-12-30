#include "MainWindow.h"
#include "resource.h"

bool MainWindow::Register(const char* name, HINSTANCE hInstance)
{
	WNDCLASSEX tag = {};
	tag.cbSize = sizeof(WNDCLASSEX);
	tag.style = CS_HREDRAW | CS_VREDRAW;
	tag.lpfnWndProc = windowProc;
	tag.cbClsExtra = 0;
	tag.cbWndExtra = 0;
	tag.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON2), 1, 32, 32, NULL));
	tag.hCursor = LoadCursor(NULL, IDC_ARROW);
	tag.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	tag.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	tag.lpszClassName = "MainWindow";
	tag.hInstance = hInstance;
	tag.hIconSm = 0;
	return RegisterClassEx(&tag) != 0;
}

bool MainWindow::Create(const char* name, HINSTANCE hInstance)
{
	handle = CreateWindowExA(0, "MainWindow", name, WS_OVERLAPPEDWINDOW, 50, 50, 1000, 800, 0, 0, hInstance, this);
	LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
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
		SetWindowLong(hnd, GWL_USERDATA, reinterpret_cast<LONG>(that));
		return DefWindowProc(hnd, message, wParam, lParam);
	}
	MainWindow* that = reinterpret_cast<MainWindow*>(GetWindowLong(hnd, GWL_USERDATA));
	switch (message)
	{
	case WM_CREATE:
		that->handle = hnd;
		return DefWindowProc(hnd, message, wParam, lParam);
	case WM_CLOSE:
		that->OnClose();
		break;
	case WM_DESTROY:
		that->OnDestroy();
		break;
	default:
		return DefWindowProc(hnd, message, wParam, lParam);
	}
	return 0;
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
