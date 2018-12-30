#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "resource.h"

/*
Dialog - выбор цвета
MainMenu - сохранить / загрузить
PopupMenu - очистить поле / сгенерировать полностью
ToolBar - всякие кнопки
Button - всякие кнопки
RadioButton - рисовать карандашом / заполнить область случайно
CheckBox - остановить когда останется одна раса
ScrollBar - на панели со статистикой населения
StatusBar - поколение
ComboBox - скорость обновления поля
ListBox - красные / зелёные / синие / нейтральные / случайные / кастомные
EditBox - размер поля, плотность населения
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
