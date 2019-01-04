#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "MainWindow.h"
#include <stdlib.h>
#include <time.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));
	MainWindow window = MainWindow();
	window.Register("Window", hInstance);
	window.Create("Life", hInstance);
	window.Show();
	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return (int)msg.wParam;
}
