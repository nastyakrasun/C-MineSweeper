// WndPrMinesweeperSelf.cpp : Определяет точку входа для приложения.

#include "framework.h"
#include "WndPrMinesweeperSelf.h"
#include <windows.h>
// для отображения шифрования
#include <windowsx.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
//файлы заголовков среды выполнения Си
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//вставить в правильное место
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN //исключить редко используемые функции

#define MAX_LOADSTRING 100
#define N 10//количество строк
#define M 10//количество столбцов
#define MAX_NUM_RECORDS 10

struct Record {
    char name[20];
    int minedCells;
    int steps;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int minute;
    unsigned int second;
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

Record records[MAX_NUM_RECORDS + 1];
int numRecords = 0;//считаем рекорды
int showMode = 1;

// добавляем изображения
BITMAP bm_cell, bm_cell1, bm_cell2, bm_cell3, bm_field, bm_mine, bm_tick;
HBITMAP hbm_cell, hbm_cell1, hbm_cell2, hbm_cell3, hbm_field, hbm_mine, hbm_tick;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// массив игрового поля
int a[N][M] = {
    {0,1,1,1,0, 0,0,0,0,0},//коды ячеек: 0 - свободна, 9 - мина, 1, 2, 3...<9 - количество мин рядом, 10 - помеченная на мину ячейка
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

int InitialArray[N][M]; // массив исходных значений чтобы получать оттуда коды ячеек, с которых сняли пометку мины TickCell()

char filenameStatus[4][80] = { "status.txt", //сюда сохраняем состояние
                               "level1.txt", //отсюда берем уровни
                               "level2.txt",
                               "level3.txt" };

char filenameRecords[80] = "records.txt";

//размер одной ячейки
int sizeX = 32;
int sizeY = 32;

//счетчики шагов и мин
int steps = 0;
int minedCells;
//int tickedCells; //счетчик помеченных ячеек TickCell()
int level;

//переходим к отрисовке - DrawField()

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

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WNDPRMINESWEEPERSELF));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_WNDPRMINESWEEPERSELF);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
 // функция для отрисовки битмапа (растрового изображения) на заданном контексте устройства (HDC) в Windows
void drawBitmap(HDC dc, BITMAP bmi, HBITMAP hbm, int x, int y, int w, int h)
{
    HDC cdc = CreateCompatibleDC(dc);
    SelectObject(cdc, hbm);
    SetStretchBltMode(dc, HALFTONE);
    if (w == 0) {
        w = bmi.bmWidth;
    }
    if (h == 0)
    {
        h = bmi.bmHeight;
    }
    StretchBlt(dc, x, y, w, h, cdc, 0, 0, bmi.bmWidth, bmi.bmHeight, SRCCOPY);//Отрисовываем ранее загруженный файл
    //DeleteObject(hbm);
    DeleteDC(cdc);
}

//функция-cчетчик мин
int CountMines() {
    int minedCells = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (a[i][j] == 9) { //ячейка содержит мину
                minedCells++;
            }
        }
    }
    return minedCells;
}


//2 создадим функию для отрисовки поля игрового
void DrawField(HDC hdc) {
    minedCells = CountMines(); //обновление количества мин перед отрисовкой

    HBRUSH hField = CreateSolidBrush(RGB(200, 200, 200));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 6, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            drawBitmap(hdc, bm_field, hbm_field, j * sizeX, i * sizeY, 0, 0);
            //FillRect(hdc, &rect, hField);
        }
    }
    //выведем на экран номер уровня, количество мин и счетчик ходов  
    char str1[] = "Уровень: ";
    char str2[] = "Содержит мин: ";
    char str3[] = "Открыто ячеек: ";
    TextOutA(hdc, 32, sizeY * (N+1), str1, strlen(str1));
    TextOutA(hdc, 32, sizeY * (N + 1) + 20, str2, strlen(str2));
    TextOutA(hdc, 32, sizeY * (N + 1)+40, str3, strlen(str3));
    //выведем количество шагов, для этого подключим <stdio.h>
    char num[3];   
    sprintf_s(num, "%d", level);
    TextOutA(hdc, 220, sizeY * (N + 1), num, strlen(num));
    sprintf_s(num, "%d", minedCells);//в отл от обычного вывод не в консоль, а в строку (_string), приставка s - безопасный (safe)
    TextOutA(hdc, 220, sizeY * (N + 1)+20, num, strlen(num));
    sprintf_s(num, "%d", steps);
    TextOutA(hdc, 220, sizeY * (N + 1)+40, num, strlen(num));

    DeleteObject(hField);
    DeleteObject(hFont);
    DeleteObject(hPen);
}//вызвать функцию DrawField(hdc); в WM_PAINT
 
 //3 создадим функию для отрисовки игровой ячейки
void DrawCell(HDC hdc) {
    /*HBRUSH hCell = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH hMine = CreateSolidBrush(RGB(0, 0, 0));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);*/
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            if (a[i][j] == 90) {
                drawBitmap(hdc, bm_mine, hbm_mine, j * sizeX, i * sizeY, 0, 0);
                //FillRect(hdc, &rect, hMine);
            }
            else {
                if (a[i][j] == 100) {
                    drawBitmap(hdc, bm_cell, hbm_cell, j * sizeX, i * sizeY, 0, 0);
                    //FillRect(hdc, &rect, hCell);
                    //Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                }            
                if (a[i][j] == 10) {   
                    drawBitmap(hdc, bm_cell1, hbm_cell1, j * sizeX, i * sizeY, 0, 0);
                    //FillRect(hdc, &rect, hCell);
                    /*Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(0, 0, 0));
                    DrawText(hdc, L"1", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
                              }
                else if (a[i][j] == 20) { 
                    drawBitmap(hdc, bm_cell2, hbm_cell2, j * sizeX, i * sizeY, 0, 0);
                    //FillRect(hdc, &rect, hCell);
                    /*Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 255, 0));
                    DrawText(hdc, L"2", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
                }
                else if (a[i][j] == 30) {
                    drawBitmap(hdc, bm_cell3, hbm_cell3, j * sizeX, i * sizeY, 0, 0);
                    //FillRect(hdc, &rect, hCell);
                    /*Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 0, 0));
                    DrawText(hdc, L"3", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
                }
                else if (a[i][j] == 900) {
                    drawBitmap(hdc, bm_tick, hbm_tick, j * sizeX, i * sizeY, 0, 0);
                    //FillRect(hdc, &rect, hCell);
                    /*Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                    SetTextColor(hdc, RGB(255, 0, 0));
                    DrawText(hdc, L"*", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
                }
            }
        }
    }
    /*DeleteObject(hCell);
    DeleteObject(hMine);
    DeleteObject(hFont);
    DeleteObject(hPen);*/
}//вызвать функцию DrawCell(hdc); в WM_PAINT

//функция для проверки победы
bool CheckVictory() {//условие победы: открыты все клетки, кроме мин
    int openedCells = 0;
    int totalCells = N * M;
    minedCells = CountMines();//отрабатывает функция подсчета количества мин

    //подсчитываем открытые клетки
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {//клетка открыта
            if (a[i][j] >= 10) {
                openedCells++;
            }
        }
    }
    //проверяем условие победы
    return (openedCells == (totalCells - minedCells));
}
//показ ячеек с минами в случае проигрыша: если ячейка содержит мину (9), обозначаем, что ячейка с миной открыта (90)
void ShowMines() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            if (a[i][j] == 9) {
                a[i][j] *= 10;
            }
        }
    }
}

// рекурсивное открытие соседних ячеек - до основной функции, тк из основной функции вызывается рекурсивная
void OpenNeighbours(int i, int j) {
    // проверка нахождения в границах игрового поля: вне границ не работаем
    if (i < 0 || i >= N || j < 0 || j >= M) {
        return;
    }

    // проверка, была ли ячейка уже открыта: если открыта, не трогаем
    if (a[i][j] >= 10) {
        return;
    }

    // открываем закрытую ячейку (0 -- 100)
    if (a[i][j] == 0) {
        a[i][j] = 100;
        steps++; // увеличиваем счетчик шагов

        // рекурсивно открываем все соседние ячейки
        OpenNeighbours(i - 1, j); // вверх
        OpenNeighbours(i + 1, j); // вниз
        OpenNeighbours(i, j - 1); // влево
        OpenNeighbours(i, j + 1); // вправо
        OpenNeighbours(i - 1, j - 1); // вверх-лево
        OpenNeighbours(i - 1, j + 1); // вверх-право
        OpenNeighbours(i + 1, j - 1); // вниз-лево
        OpenNeighbours(i + 1, j + 1); // вниз-право
    }
    else if (a[i][j] >= 1 && a[i][j] <= 8) {
        a[i][j] *= 10; // открываем ячейку с числом
        steps++; // увеличиваем счетчик шагов
    }
}


//4 функция открытия ячеек
void OpenCell(int mouseX, int mouseY) {
    //определяем, какая ячейка была нажата
    int i = mouseY / sizeY;//номер строки
    int j = mouseX / sizeX;//номер столбца

    // проверка нахождения в границах игрового поля
    if (i < 0 || i >= N || j < 0 || j >= M) {
        return;
    }
    //если мина
    if (a[i][j] == 9) {
        a[i][j] *= 10;
        //игра закончена, поражение
        MessageBox(NULL, L"Поражение\nВы открыли ячейку с миной", L"Игра окончена", MB_OK);
        //открываем все мины
        ShowMines();
    }
    //если ячейка свободна
    else if (a[i][j] == 0) {
        //a[i][j] = 100;//перерисовываем ячейку
        //steps++;//увеличиваем счетчик шагов
        //рекуррсивно открываем соседние ячейки
        OpenNeighbours(i, j);
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

//5 функция метки ячеек
void TickCell(int mouseX, int mouseY) {
    //определяем, какая ячейка была нажата
    int i = mouseY / sizeY;//номер строки
    int j = mouseX / sizeX;//номер столбца

    //проверка границ
    if (i < 0 || i >= N || j < 0 || j >= M) {
        return;
    }
    
    //если ячейка помечена (900), возвращаем её в исходное состояние
    if (a[i][j] == 900) {
        //восстанавливаем исходное значение из массива InitialArray
        a[i][j] = InitialArray[i][j];
        //tickedCells--; //уменьшаем счетчик отмеченных мин
    }
    else if (a[i][j] >= 1 && a[i][j] <= 9) {
        // если ячейка не помечена, помечаем её
        a[i][j] = 900; //перерисовываем ячейку
        //tickedCells++; //увеличиваем счетчик отмеченных мин
    }
}//вызвать функцию TickCell(hdc); в WM_RBUTTONDOWN

void DrawTickedCell(HDC hdc) {
    /*HBRUSH hTick = CreateSolidBrush(RGB(255, 0, 0));
    HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    SelectObject(hdc, hFont);
    SelectObject(hdc, hPen);*/
    for (int i = 0; i < N; i++) {//цикл обходит по строкам
        for (int j = 0; j < M; j++) {//цикл обходит по элементам строки
            RECT rect = { j * sizeX,i * sizeY,(j + 1) * sizeX,(i + 1) * sizeY };
            if (a[i][j] == 900) {
                drawBitmap(hdc, bm_tick, hbm_tick, j * sizeX, i * sizeY, 0, 0);
                //FillRect(hdc, &rect, hTick);
                /*Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
                SetTextColor(hdc, RGB(255, 0, 0));
                DrawText(hdc, L"*", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
            }
        }
    }
    /*DeleteObject(hTick);
    DeleteObject(hFont);
    DeleteObject(hPen);*/
}//вызвать функцию DrawTickedCell(hdc); в WM_PAINT

void loadStatus()
{
    FILE* f;
    fopen_s(&f, filenameStatus[0], "rt");
    if (f == NULL) {
        MessageBoxA(0, "Не удалось загрузить статус игры. Файл не найден.", "Ошибка", MB_OK);
        return; //проверка на успешное открытие файла
    }
    //восстанавливаем количество шагов и мин
    fscanf_s(f, "%d %d\n", &steps, &minedCells);
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
            InitialArray[i][j] = a[i][j]; //записываем массив исходных значений
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
    fprintf(f, "%d %d\n", steps, minedCells);

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

// 14.01.24 рекорды
void addRecord(char name[])
{
    //27.12.24 запись в одну табличку с рекордами
    if (numRecords >= MAX_NUM_RECORDS) {
        numRecords = MAX_NUM_RECORDS - 1;
    }
    strcpy_s(records[numRecords].name, name); //скопировали имя
    records[numRecords].minedCells = minedCells;
    records[numRecords].steps = steps;
    //для хранения времени
    SYSTEMTIME st;
    GetLocalTime(&st);
    //st. //пробелом смотрим поля функции st
    records[numRecords].year = st.wYear;//записываем время до секунды
    records[numRecords].month = st.wMonth;
    records[numRecords].day = st.wDay;
    records[numRecords].hour = st.wHour;
    records[numRecords].minute = st.wMinute;
    records[numRecords].second = st.wSecond;
    numRecords++;//отсортируем таблицу по макс рекордам сверху - CompareRecords
}

int CompareRecords(int index1, int index2)//передаем порядковые номера рекордов вв функцию
{
    if (records[index1].minedCells < records[index2].minedCells)//если у первого меньше золота
    {
        return -1;
    }
    if (records[index1].minedCells > records[index2].minedCells)
    {
        return 1;
    }
    if (records[index1].steps > records[index2].steps)//если золота одинаково - сравниваем шаги
    {
        return -1;
    }
    if (records[index1].steps < records[index2].steps)
    {
        return 1;
    }
    return 0;
}

//добавление рекорда с сортировкой
void insertRecord(char name[])
{
    strcpy_s(records[numRecords].name, name); //скопировали имя
    records[numRecords].minedCells = minedCells;
    records[numRecords].steps = steps;
    //для хранения времени
    SYSTEMTIME st;
    GetLocalTime(&st);
    //st. //пробелом смотрим поля функции st
    records[numRecords].year = st.wYear;//записываем время до секунды
    records[numRecords].month = st.wMonth;
    records[numRecords].day = st.wDay;
    records[numRecords].hour = st.wHour;
    records[numRecords].minute = st.wMinute;
    records[numRecords].second = st.wSecond;
    //если результат хороштй - двигаем вверх
    int i = numRecords;
    while (i > 0) {
        if (CompareRecords(i - 1, i) < 0) {
            Record temp = records[i];
            records[i] = records[i - 1];
            records[i - 1] = temp;
            i--;
        }
    }
    if (numRecords < MAX_NUM_RECORDS) {
        numRecords++;
    }
}

//cоздадим функцию для отображения таблицы - DrawRecords
void drawRecords(HDC hdc) {
    HFONT hFont = CreateFontA(16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Courier New");
    SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 64, 64));
    char str1[] = "| №  | Дата       | Время    | Имя                  | Мин всего | Ходов |";
    TextOutA(hdc, 10, 50, str1, strlen(str1));

    for (int i = 0; i < numRecords; i++) {
        char str2[100];
        sprintf_s(str2, "| %2d | %02d.%02d.%4d | %02d:%02d:%02d | %-20s | %9d | %5d |", //%2d  - целое число длиной 2 символа, %02d - заполняет оставшиеся символы нулями %-20s - 20 символов с выравниванием по левому краю
            i + 1, records[i].day, records[i].month, records[i].year,
            records[i].hour, records[i].minute, records[i].second,
            records[i].name, records[i].minedCells, records[i].steps
        );
        TextOutA(hdc, 10, 50 + (i + 1) * 24, str2, strlen(str2));
    }
}//showmode режим отображения бинарный 0 - ничего 1 - игровое поле

//функция шифрования одного символа
int encode(char c, int key)
{
    int newCh = c;
    //если переданная буква заглавная
    if (c >= 'A' && c <= 'Z') {
        //шифруем Цезарем
        newCh += key;
        if (newCh > 'Z') {
            newCh = 'A' + (newCh - 'Z' - 1);
        }
    }
    //если переданная буква маленькая
    if (c >= 'a' && c <= 'z') {
        //шифруем Цезарем
        newCh += key;
        if (newCh > 'z') {
            newCh = 'a' + (newCh - 'z' - 1);
        }
    }
    //если передана цифра
    if (c >= '0' && c <= '9') {
        //шифруем Цезарем
        newCh += key;
        if (newCh > '9') {
            newCh = '0' + (newCh - '9' - 1);
        }
    }
    return newCh; //на выходе возвращаем значение функции
}

//функция дешифрования одного символа
int decode(char c, int key)
{
    int newCh = c;
    //если переданная буква заглавная
    if (c >= 'A' && c <= 'Z') {
        //шифруем Цезарем
        newCh -= key;
        if (newCh < 'A') {
            newCh = 'Z' - ('A' - newCh - 1);
        }
    }
    //если переданная буква маленькая
    if (c >= 'a' && c <= 'z') {
        //шифруем Цезарем
        newCh -= key;
        if (newCh < 'a') {
            newCh = 'z' - ('a' - newCh - 1);
        }
    }
    //если передана цифра
    if (c >= '0' && c <= '9') {
        //шифруем Цезарем
        newCh -= key;
        if (newCh < '0') {
            newCh = '9' - ('0' - newCh - 1);
        }
    }
    return newCh; //на выходе возвращаем значение функции
}

//зашифровать всю строку циклом while по всей строке
void EncodeString(char str[], int key) {
    //создаем счетчик
    int i = 0;
    //пока не доходим до конца строки (0 - конец строки)
    while (str[i] != 0) {
        str[i] = encode(str[i], key);
        //увеличиваем счетчик
        i++;
    }
}

//расшифровать всю строку циклом while по всей строке
void DecodeString(char str[], int key) {
    //создаем счетчик
    int i = 0;
    //пока не доходим до конца строки (0 - конец строки)
    while (str[i] != 0) {
        str[i] = decode(str[i], key);
        //увеличиваем счетчик
        i++;
    }
}

// нужно загрузить таблицу рекордов - лаба 20 - saveRecords и loadRecords (после saveRecords, чтобы было что грузить)
// 16/01/24 шифруем и расшифровываем
void saveRecords() {
    FILE* f;
    fopen_s(&f, filenameRecords, "wt");
    fprintf(f, "%d", numRecords);
    for (int i = 0; i < numRecords; i++) {
        char encStr[255];
        sprintf_s(encStr, "%s\n %d %d %d %d %d %d %d %d\n",
            records[i].name,
            records[i].minedCells,
            records[i].steps,
            records[i].year,
            records[i].month,
            records[i].day,
            records[i].hour,
            records[i].minute,
            records[i].second);
        EncodeString(encStr, 3);
        fprintf(f, "%s", encStr);
    }
    fclose(f);
}

//расшифровываем
void loadRecords() {
    FILE* f;
    fopen_s(&f, filenameRecords, "rt");
    fscanf_s(f, "%d\n", &numRecords);
    for (int i = 0; i < numRecords; i++) {
        fgets(records[i].name, 80, f);
        for (int j = 0; j < strlen(records[i].name); j++) {
            if (records[i].name[j] == '\n') {
                records[i].name[j] = 0;
            }
        }
        //расшифровываем
        DecodeString(records[i].name, 3);
        char encStr[255];
        fgets(encStr, 255, f);
        DecodeString(encStr, 3);

        sscanf_s(encStr, "%d%d%d%d%d%d%d%d\n",
            &records[i].minedCells,
            &records[i].steps,
            &records[i].year,
            &records[i].month,
            &records[i].day,
            &records[i].hour,
            &records[i].minute,
            &records[i].second);
    }
    fclose(f);
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
      //CW_USEDEFAULT, 0, 336, 500, nullptr, nullptr, hInstance, nullptr);
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
        static HWND hEdit1;
        static HWND hAddBtn;

    case WM_CREATE: {
        hbm_cell = (HBITMAP)LoadImageA(0, "C:\\Temp\\Cell.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_cell1 = (HBITMAP)LoadImageA(0, "C:\\Temp\\Cell1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_cell2 = (HBITMAP)LoadImageA(0, "C:\\Temp\\Cell2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_cell3 = (HBITMAP)LoadImageA(0, "C:\\Temp\\Cell3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_field = (HBITMAP)LoadImageA(0, "C:\\Temp\\Field.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_mine = (HBITMAP)LoadImageA(0, "C:\\Temp\\Mine.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        hbm_tick = (HBITMAP)LoadImageA(0, "C:\\Temp\\Tick.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);//Загружаем файл
        GetObject(hbm_cell, sizeof(BITMAP), &bm_cell);
        GetObject(hbm_cell1, sizeof(BITMAP), &bm_cell1);
        GetObject(hbm_cell2, sizeof(BITMAP), &bm_cell2);
        GetObject(hbm_cell3, sizeof(BITMAP), &bm_cell3);
        GetObject(hbm_field, sizeof(BITMAP), &bm_field);
        GetObject(hbm_mine, sizeof(BITMAP), &bm_mine);
        GetObject(hbm_tick, sizeof(BITMAP), &bm_tick);

        hInst = ((LPCREATESTRUCT)lParam)->hInstance;
        hEdit1 = CreateWindowA("edit", "Noname", WS_CHILD | WS_VISIBLE
            | WS_BORDER | ES_LEFT, 650, 50, 160, 20, hWnd, 0, hInst, NULL);
        ShowWindow(hEdit1, SW_SHOWNORMAL);
        hAddBtn = CreateWindowA("button", "Запомнить", WS_CHILD | WS_VISIBLE |
            WS_BORDER, 650, 100, 160, 24, hWnd, 0, hInst, NULL);
        ShowWindow(hAddBtn, SW_SHOWNORMAL);
        srand(time(0));
        SetTimer(hWnd, 1, 100, NULL);
        loadRecords();
        break;
    }

    case WM_LBUTTONDOWN: {
        /*POINT pt;//получаем координаты курсора
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt); //преобразуем в клиентские координаты
        //вызываем функцию открытия ячейки
        OpenCell(pt.x, pt.y); //очень медленно загружается
        */
        //извлечение координат нажатия мыши
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        //вызов функции OpenCell с координатами мыши
        OpenCell(mouseX, mouseY);
        InvalidateRect(hWnd, NULL, false); //перерисовываем окно false чтобы перерисовка осуществлялась плавно
        break;
    }

    case WM_RBUTTONDOWN: { // Обработка нажатия правой кнопки мыши
        /*POINT pt;//получаем координаты курсора
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt); //преобразуем в клиентские координаты
        //вызываем функцию пометки ячейки
        TickCell(pt.x, pt.y); //очень медленно загружается
        */

        //извлечение координат нажатия мыши
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        //вызов функции TickCell с координатами мыши
        TickCell(mouseX, mouseY);
        InvalidateRect(hWnd, NULL, false); //перерисовываем окно false чтобы перерисовка осуществлялась плавно
        break;
    }

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
                       if (wParam == VK_TAB) { // переключение между игровым полем и рекордами
                           showMode = 1 - showMode; // переключаем между 1 и 0
                           InvalidateRect(hWnd, NULL, true);
                       }
                       break;
    }

    case WM_COMMAND:
        {
        if (lParam == (LPARAM)hAddBtn)
        {
            char name[80];
            GetWindowTextA(hEdit1, name, 80);
            MessageBoxA(hWnd, "Рекорд добавлен", "Добавление рекорда", MB_OK);
            insertRecord(name);
            SetFocus(hWnd);
        }
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
            if (showMode == 1) {
            DrawField(hdc);
            DrawCell(hdc);
            DrawTickedCell(hdc);
            }
            else {
                drawRecords(hdc);
            }
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:

        saveRecords();
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
