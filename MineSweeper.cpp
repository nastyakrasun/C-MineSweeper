// WndPrMinesweeperSelf.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "WndPrMinesweeperSelf.h"
#include <windows.h>
#include <stdio.h>

#define MAX_LOADSTRING 100
#define N 10//количество строк
#define M 10//количество столбцов

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int a[N][M] = {
    {0,1,1,1,0, 0,0,0,0,0},//коды ячеек: 0 - свободна, 9 - мина, 1, 2, 3...<9 - количество мин рядом
    {0,1,9,1,0, 0,0,0,0,0},
    {0,1,1,1,0, 0,0,0,0,0},
    {0,0,0,0,1, 1,1,0,0,0},
    {0,0,0,0,1, 9,1,0,0,0},
    {0,0,0,0,1, 1,1,0,0,0},
    {0,0,0,0,0, 0,1,1,1,0},
    {0,0,0,0,0, 0,1,9,1,0},
    {0,0,0,0,0, 0,1,1,1,0},
    {0,0,0,0,0, 0,0,0,0,0},
};
char filenameStatus[4][80] = { "status.txt", //сюда сохраняем состояние
                               "level1.txt", //отсюда берем уровни
                               "level2.txt",
                               "level3.txt" };

//размер одной ячейки
int sizeX = 32;
int sizeY = 32;
//счетчики шагов и мин
int steps = 0;
int mine = 0;
int level;
int mineCells;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WNDPRMINESWEEPERSELF, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WNDPRMINESWEEPERSELF));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WNDPRMINESWEEPERSELF));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WNDPRMINESWEEPERSELF);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//функция-cчетчик мин
int CountMines() {
    int mineCells = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (a[i][j] == 9) { //ячейка содержит мину
                mineCells++;
            }
        }
    }
    return mineCells;
}

//2 создадим функию для отрисовки поля игрового
void DrawField(HDC hdc) {
    mineCells = CountMines(); //обновление количества мин перед отрисовкой

    HBRUSH hField = CreateSolidBrush(RGB(200, 200, 200));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 6, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            FillRect(hdc, &rect, hField);
        }
    }
    //выведем на экран номер уровня, количество мин и счетчик ходов  
    char str1[] = "Уровень: ";
    char str2[] = "Содержит мин: ";
    char str3[] = "Сделано ходов: ";
    TextOutA(hdc, 32, sizeY * (N+1), str1, strlen(str1));
    TextOutA(hdc, 32, sizeY * (N + 1) + 20, str2, strlen(str2));
    TextOutA(hdc, 32, sizeY * (N + 1)+40, str3, strlen(str3));
    //выведем количество шагов, для этого подключим <stdio.h>
    char num[3];   
    sprintf_s(num, "%d", level);
    TextOutA(hdc, 220, sizeY * (N + 1), num, strlen(num));
    sprintf_s(num, "%d", mineCells);//в отл от обычного вывод не в консоль, а в строку (_string), приставка s - безопасный (safe)
    TextOutA(hdc, 220, sizeY * (N + 1)+20, num, strlen(num));
    sprintf_s(num, "%d", steps);
    TextOutA(hdc, 220, sizeY * (N + 1)+40, num, strlen(num));

    DeleteObject(hField);
    DeleteObject(hFont);
    DeleteObject(hPen);
}//вызвать функцию DrawField(hdc); в WM_PAINT
 
 //3 создадим функию для отрисовки игровой ячейки
void DrawCell(HDC hdc) {
    HBRUSH hCell = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hMine = CreateSolidBrush(RGB(0, 0, 0));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            if (a[i][j] == 90) {
                FillRect(hdc, &rect, hMine);
            }
            else {
                if (a[i][j] == 100) {
                    FillRect(hdc, &rect, hCell);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                }            
                if (a[i][j] == 10) {   
                    FillRect(hdc, &rect, hCell);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                SetTextColor(hdc, RGB(0, 0, 0));
                DrawText(hdc, L"1", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                              }
                else if (a[i][j] == 20) { 
                    FillRect(hdc, &rect, hCell);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 255, 0));
                    DrawText(hdc, L"2", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                else if (a[i][j] == 30) {
                    FillRect(hdc, &rect, hCell);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 0, 0));
                    DrawText(hdc, L"3", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
                else if (a[i][j] == 900) {
                    FillRect(hdc, &rect, hCell);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 0, 0));
                    DrawText(hdc, L"*", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }
        }
    }
    DeleteObject(hCell);
    DeleteObject(hMine);
    DeleteObject(hFont);
    DeleteObject(hPen);
}//вызвать функцию DrawCell(hdc); в WM_PAINT

void TickCell(HDC hdc) {
    HBRUSH hTick = CreateSolidBrush(RGB(255, 0, 0));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            if (a[i][j] == 900) {
                    FillRect(hdc, &rect, hTick);
                    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 0, 0));
                    DrawText(hdc, L"*", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }
        }    
    DeleteObject(hTick);
    DeleteObject(hFont);
    DeleteObject(hPen);
}//вызвать функцию TickCell(hdc); в WM_PAINT

//функция для проверки победы
bool CheckVictory() {//условие победы: открыты все клетки, кроме мин
    int openedCells = 0;
    int totalCells = N * M;
    mineCells = CountMines();//отрабатывает функция подсчета количества мин

    //подсчитываем открытые клетки
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {//клетка открыта
            if (a[i][j] >= 10) {
                openedCells++;
            }
        }
    }
    //проверяем условие победы
    return (openedCells == (totalCells - mineCells));
}

//4 функция открытия ячеек
void OpenCell(int mouseX, int mouseY) {
    //определяем, какая ячейка была нажата
    int i = mouseY / sizeY;//номер строки
    int j = mouseX / sizeX;//номер столбца

    //проверка границ
    if (i < 0 || i >= N || j < 0 || j >= M) {
        return;//нажатие вне игрового поля
    }
    //если мина
    if (a[i][j] == 9) {
        a[i][j] *= 10;
        //игра закончена, поражение
        MessageBox(NULL, L"Поражение\nВы открыли ячейку с миной", L"Игра окончена", MB_OK);
    }
    //если ячейка свободна
    else if (a[i][j] == 0) {
        a[i][j] = 100;//перерисовываем ячейку
        steps++;//увеличиваем счетчик шагов
    }
    //если ячейка содержит число
    else if (a[i][j] >= 1 && a[i][j] <= 8) {
        a[i][j] *= 10;//перерисовываем ячейку
        steps++;//увеличиваем счетчик шагов
    }
    //проверка на победу
    if (CheckVictory()) {
        MessageBox(NULL, L"Поздравляем!\nУровень пройден", L"Игра окончена", MB_OK);
    }
}

////5 функция метки ячеек
//void TickCell(int mouseX, int mouseY) {
//    //определяем, какая ячейка была нажата
//    int i = mouseY / sizeY;//номер строки
//    int j = mouseX / sizeX;//номер столбца
//
//    //проверка границ
//    if (i < 0 || i >= N || j < 0 || j >= M) {
//        return;//нажатие вне игрового поля
//    }
//    else  {
//        a[i][j] = 900;//перерисовываем ячейку
//        mine++;//увеличиваем счетчик отмеченных мин
//    }
//}

void loadStatus()
{
    FILE* f;
    fopen_s(&f, filenameStatus[0], "rt");
    if (f == NULL) {
        MessageBoxA(0, "Не удалось загрузить статус игры. Файл не найден.", "Ошибка", MB_OK);
        return; //проверка на успешное открытие файла
    }

    //восстанавливаем количество шагов и мин
    fscanf_s(f, "%d %d\n", &steps, &mine);
    int i = 0;
    while (i < N) {
        int j = 0;
        while (j < M) {
            fscanf_s(f, "%d", &a[i][j]);
            j++;
        }
        i++;
    }
    fclose(f);
}

void loadLevel(int level)
{
    if (level < 1 || level > 3) {
        MessageBoxA(0, "Некорректный уровень. Пожалуйста, выберите уровень от 1 до 3.", "Ошибка", MB_OK);
        return; //проверка на корректность уровня
    }
    FILE* f;
    if (fopen_s(&f, filenameStatus[level], "rt") != 0) {
        MessageBoxA(0, "Не удалось загрузить уровень игры. Файл не найден.", "Ошибка", MB_OK);
        return; //выходим из функции, если файл не найден
    }
    
    steps = 0;//сброс счетчика шагов при загрузке нового уровня
    ::level = level;//получение номера уровня
   
    int i = 0;
    while (i < N) {
        int j = 0;
        while (j < M) {
            fscanf_s(f, "%d", &a[i][j]);
            j++;
        }
        i++;
    }
    fclose(f);
}

//прописываем процедуру сохранения статуса
void saveStatus()
{
    FILE* f;
    fopen_s(&f, filenameStatus[0], "wt");//открываем на запись
    if (f == NULL) {
        MessageBoxA(0, "Не удалось сохранить статус игры.", "Ошибка", MB_OK);
        return; //проверка на успешное открытие файла
    }

    //сохраняем количество шагов и мин
    fprintf(f, "%d %d\n", steps, mine);

    int i = 0;
    while (i < N) {//пока строка не кончится бежим по элементам строки
        int j = 0;
        while (j < M) {//и записываем числа через пробел, пробел важен для отделения элементов
            fprintf(f, "%d ", a[i][j]);
            j++;//переходим к след элементу нашей строки
        }
        fprintf(f, "\n");//добавляем перенос строки после каждой строки
        i++;//увеличиваем номер строки для перехода к след строке
    }
    fclose(f);//закрыаем файл после работы с ним
    MessageBoxA(0, "Файл успешно записан", "Всё получилось", MB_OK);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 336, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN: {
        POINT pt;//получаем координаты курсора
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt); //преобразуем в клиентские координаты

        //вызываем функцию открытия ячейки
        OpenCell(pt.x, pt.y);
        InvalidateRect(hWnd, NULL, TRUE); //перерисовываем окно
        break;
    }
    //case WM_RBUTTONDOWN: { // Обработка нажатия правой кнопки мыши
    //    int mouseX = LOWORD(lParam);
    //    int mouseY = HIWORD(lParam);
    //    TickCell(mouseX, mouseY); // Вызов функции открытия ячейки
    //    InvalidateRect(hWnd, NULL, TRUE); // Перерисовать окно
    //    return 0;
    //    break;
    //}
    case WM_KEYDOWN: {
                       if (wParam == 0x53) { // нажата клавиша S 
                           saveStatus();
                           InvalidateRect(hWnd, NULL, true);
                       }
                       if (wParam == 0x5A) { // нажата клавиша Z
                           loadStatus();
                           InvalidateRect(hWnd, NULL, true);
                       }
                       if (wParam == 0x31) {
                           loadLevel(1);
                           InvalidateRect(hWnd, NULL, true);
                       }
                       if (wParam == 0x32) {
                           loadLevel(2);
                           InvalidateRect(hWnd, NULL, true);
                       }
                       if (wParam == 0x33) {
                           loadLevel(3);
                           InvalidateRect(hWnd, NULL, true);
                       }
                       break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawField(hdc);
            DrawCell(hdc);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
