# Life RGB
Клеточный автомат "Жизнь" с цветовым геном клеток.

## Техническое задание
- программа должна содержать объектно-ориентированную модель заданной предметной области;
- программа должна содержать следующие элементы управления: 
  - Dialog;
  - MainMenu;
  - PopupMenu;
  - ToolBar;
  - Button;
  - RadioButton;
  - CheckBox;
  - ScrollBar;
  - StatusBar;
  - ComboBox;
  - ListBox;
  - EditBox;
- элементы управления должны содержать контекстные подсказки (ToolTip или Hint);
- для создания элементов управления Windows использовать функции и объекты WinAPI;
- создание и обработка элементов управления должны осуществляться без использования ClassWizard;
- приложение должно быть многопоточным;

## Условие задачи
Основная задача этого объектно-ориентированного проекта – обеспечить удобную оболочку для описания "окрестности влияния" и задания функции перехода – в какое состояние переходит каждая клетка в зависимости от состояний лично ее и состояний соседей, попадающих в окрестность влияния. Функции могут быть как детерминированные, так и вероятностные. Работа клеточного автомата должна быть выведена на экран. Конструкторы должны обеспечивать различные способы задания исходной конфигурации – считывание из файла, вероятностное задание, блочное задание. Процесс обновления может быть синхронным и пуассоновским.

## Скриншоты
### Главное окно
![Главное окно](https://i.imgur.com/YwDyN7V.png)
