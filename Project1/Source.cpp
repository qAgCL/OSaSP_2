#include <Windows.h> // заголовочный файл, содержащий функции API
#include <math.h>
#include <time.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
const int MainWindowHeight = 800;
const int MainWindowWidth = 1200;
const int NumberOfCols = 10;
const int NumberOfRows = 7;
const int MaxText = 12;
const int Lenght = 200;
const int PenWidth = 4;
int MinWindowHeight;
const int MinCellHeight = 30;
int CellWidth;
const char* FileName = "test.txt";
int NumberText;
int IDDialog;
HWND hEdt1 = NULL, hEdt2=NULL;
POINT EditCellNumb;
bool EditFlag = TRUE;
const HBRUSH WindowBackground = CreateSolidBrush(RGB(255,255,255));
const HBRUSH ObjectBackground = CreateSolidBrush(RGB(0, 0, 0));
const HFONT NewFont = CreateFont(18, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, "Calibri");
const HPEN MyPen = CreatePen(PS_SOLID, PenWidth, RGB(0, 0, 0));
void CreatePointsMap(RECT ClientRect, RECT Map[NumberOfRows][NumberOfCols]);
RECT Table[NumberOfRows][NumberOfCols];
char CellInfo[NumberOfRows][NumberOfCols][Lenght] = { 's' };
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, // дескриптор экземпляра приложения
	HINSTANCE hPrevInstance, // в Win32 не используется
	LPSTR lpCmdLine, // нужен для запуска окна в режиме командной строки
	int nCmdShow) // режим отображения окна
{
	WNDCLASSEX WinClass;
	const char* ClassName = "MainClass";
	WinClass.cbSize = sizeof(WinClass);
	WinClass.lpfnWndProc = (WNDPROC)WndProc;
	WinClass.style = 0;
	WinClass.cbClsExtra = 0;
	WinClass.cbWndExtra = 0;
	WinClass.hInstance = hInstance;
	WinClass.hIcon = LoadIcon(NULL, IDI_ERROR);
	WinClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WinClass.hbrBackground = WindowBackground;
	WinClass.lpszMenuName = NULL;
	WinClass.lpszClassName = ClassName;
	WinClass.hIconSm = NULL;
	if (!RegisterClassEx(&WinClass)) {
		MessageBox(NULL,
			"Не получилось зарегистрировать класс!",
			"Ошибка", MB_OK);
		return NULL; 
	}
	ifstream  file(FileName, ios::in | ios::binary);
	if (!file) return 1;
	for (int i = 0; i < NumberOfRows; i++)
	{
		for (int j = 0; j < NumberOfCols; j++) {
			file.getline(CellInfo[i][j], Lenght - 1, '\n');
		}
	}
	file.close();
	for (int i = 0; i < NumberOfRows; i++)
	{
		for (int j = 0; j < NumberOfCols; j++) {
			for (int k = 0; k < Lenght; k++) {
				if (CellInfo[i][j][k] == '\r') {
					CellInfo[i][j][k] = '\0';
					break;
				}
			}
		}
	}
	const char* WindowName = "MainWindow";
	HWND MainWindow = CreateWindowEx(0, ClassName, WindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, MainWindowWidth, MainWindowHeight, NULL, NULL, hInstance, NULL);
	if (!MainWindow) {
		MessageBox(NULL,
			"Не получилось создать окно",
			"Ошибка", MB_OK);
		return NULL;
	}

	ShowWindow(MainWindow, nCmdShow);
	UpdateWindow(MainWindow);
	MSG RecMessage;
	while (GetMessage(&RecMessage, NULL, 0, 0)) {
		TranslateMessage(&RecMessage);
		DispatchMessage(&RecMessage);
	}
	return RecMessage.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	HDC hDC;
	char BufStr[Lenght] = { '\0' };
	POINTS point;
	HINSTANCE hInst;
	int max=0,min=0,size = 0;
	RECT test;
	RECT rc;
	HDC hdcMem;
	HBITMAP hbmMem, hbmOld;
	HBRUSH hbrBkGnd;
	HFONT hfntOld;

	PAINTSTRUCT PaintStruct;
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(NULL);
		break;
	case WM_SIZE:
		if (!EditFlag) {
			GetWindowText(hEdt1, BufStr, 200);
			strncpy(CellInfo[EditCellNumb.x][EditCellNumb.y], BufStr, 200);
			EditFlag = TRUE;
			DestroyWindow(hEdt1);
			InvalidateRect(hWnd, NULL, TRUE);
		}
	    InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &PaintStruct);
		GetClientRect(hWnd, &test);
		POINT testP;

		hdcMem = CreateCompatibleDC(hDC);
		hbmMem = CreateCompatibleBitmap(hDC,
			test.right - test.left,
			test.bottom - test.top);
		SelectObject(hdcMem, hbmMem);
		hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

		hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(hdcMem, &test, hbrBkGnd);
		DeleteObject(hbrBkGnd);

		SelectObject(hdcMem, MyPen);
		SelectObject(hdcMem, NewFont);
		CreatePointsMap(test, Table);
		for (int i = 0; i < NumberOfRows; i++) {
			for (int j = 0; j < NumberOfCols; j++) {
				RECT LocalRect;
				Table[i][j].top= min;
				LocalRect = Table[i][j];
				LocalRect.top += PenWidth;
				LocalRect.left += PenWidth;
				LocalRect.right -= PenWidth;
			    size = DrawText(hdcMem, CellInfo[i][j], -1, &LocalRect, DT_EDITCONTROL | DT_WORDBREAK | DT_NOCLIP);
				size +=PenWidth;
				if (j == 0) { max = size; };
				if (size > max) {
					max = size;
				}
			}
			if (max < MinCellHeight) {
				max = MinCellHeight;
			}
			for (int j = 0; j < NumberOfCols; j++) {
				RECT LocalRect;
				MoveToEx(hdcMem, Table[i][j].left-PenWidth/2, Table[i][j].top + PenWidth / 2, &testP);
				LineTo(hdcMem, Table[i][j].right , Table[i][j].top + PenWidth / 2);
				LocalRect = Table[i][j];
				LocalRect.bottom += max;
				LocalRect.top += PenWidth;
				LocalRect.left += PenWidth;
				LocalRect.right -= PenWidth;
				Table[i][j].bottom = Table[i][j].top + max;
				MoveToEx(hdcMem, Table[i][j].left-PenWidth/2, Table[i][j].top, &testP);
				LineTo(hdcMem, Table[i][j].left- PenWidth / 2, Table[i][j].bottom+ PenWidth / 2);
			}
			min += max;
			MoveToEx(hdcMem, Table[i][NumberOfCols -1].left - PenWidth / 2+CellWidth, Table[i][NumberOfCols - 1].top, &testP);
			LineTo(hdcMem, Table[i][NumberOfCols -1].left - PenWidth / 2 + CellWidth, Table[i][NumberOfCols - 1].bottom+ PenWidth / 2);
		} 
		MoveToEx(hdcMem, 0, Table[NumberOfRows - 1][NumberOfCols - 1].bottom+PenWidth/2, &testP);
		LineTo(hdcMem, Table[NumberOfRows - 1][NumberOfCols - 1].right, Table[NumberOfRows - 1][NumberOfCols - 1].bottom+ PenWidth/2);
		MinWindowHeight = Table[NumberOfRows - 1][NumberOfCols - 1].bottom + PenWidth / 2;
		BitBlt(hDC, test.left, test.top,test.right - test.left,test.bottom - test.top,hdcMem,0, 0,SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteObject(hbmMem);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &PaintStruct);
		break;
	case WM_LBUTTONDOWN:
		if (EditFlag) {
			point = MAKEPOINTS(lParam);
			bool LocalFlag = FALSE;
			RECT ptsd;
			for (int i = 0; i < NumberOfRows; i++) {
				for (int j = 0; j < NumberOfCols; j++) {
					if ((point.x >= Table[i][j].left) && (point.x < Table[i][j].right) && (point.y >= Table[i][j].top) && (point.y <= Table[i][j].bottom)) {
						ptsd = Table[i][j];
						strncpy(BufStr, CellInfo[i][j], Lenght);
						EditCellNumb.x = i;
						EditCellNumb.y = j;
						LocalFlag = TRUE;
					}
				}
			}
			if (LocalFlag) {
				hInst = GetModuleHandle(NULL);
				hEdt1 = CreateWindow("edit", "0",
					WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE, ptsd.left, ptsd.top + PenWidth, ptsd.right - ptsd.left, ptsd.bottom - ptsd.top - PenWidth,
					hWnd, (HMENU)IDDialog, hInst, NULL);
				SetWindowText(hEdt1, BufStr);
				SendMessage(hEdt1, WM_SETFONT, (WPARAM)(NewFont), 0L);
				ShowWindow(hEdt1, SW_SHOWNORMAL);
				EditFlag = FALSE;
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (!EditFlag){
		GetWindowText(hEdt1, BufStr, 200);
		strncpy(CellInfo[EditCellNumb.x][EditCellNumb.y], BufStr, 200);
		EditFlag = TRUE;
		DestroyWindow(hEdt1);
		InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
	/*case WM_CTLCOLOREDIT:
		if ((HWND)lParam == GetDlgItem(hWnd, IDDialog))
		{
			SetBkMode((HDC)wParam, 0);
			return (INT_PTR)GetStockObject(LTGRAY_BRUSH);
		}*/
	case WM_GETMINMAXINFO:
	{
       LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
       lpMMI->ptMinTrackSize.x = 500;
       lpMMI->ptMinTrackSize.y = MinWindowHeight+40;
	}
		break;
	default: 
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return NULL;
}

void CreatePointsMap(RECT ClientRect, RECT Map[NumberOfRows][NumberOfCols]) {
	CellWidth = (ClientRect.right - ClientRect.left-PenWidth) / NumberOfCols;
	int x, y;	
	for (int i = 0; i < NumberOfRows; i++) {
		x = 0;
		for (int j = 0; j < NumberOfCols; j++) {
			Map[i][j].right = CellWidth+x;
			Map[i][j].left = PenWidth+x;
			Map[i][j].top = 0;
			Map[i][j].bottom = 0;
			x += CellWidth;
		}
	}
}