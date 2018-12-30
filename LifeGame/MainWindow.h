#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "resource.h"

/*
Dialog - ����� �����
MainMenu - ��������� / ���������
PopupMenu - �������� ���� / ������������� ���������
ToolBar - ������ ������
Button - ������ ������
RadioButton - �������� ���������� / ��������� ������� ��������
CheckBox - ���������� ����� ��������� ���� ����
ScrollBar - �� ������ �� ����������� ���������
StatusBar - ���������
ComboBox - �������� ���������� ����
ListBox - ������� / ������ / ����� / ����������� / ��������� / ���������
EditBox - ������ ����, ��������� ���������
*/

class MainWindow
{
public:
	MainWindow() : handle(0) {}
	static bool Register(const char*, HINSTANCE);
	bool Create(const char*, HINSTANCE);
	void Show();

protected:
	void OnDestroy();
	void OnClose();
private:
	HWND handle;
	LPARAM lparam;
	static LRESULT __stdcall windowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
};
