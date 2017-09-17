#include <Windows.h>
#include <tchar.h>
#include <string>
#include <stdio.h>
#include <time.h>

//------------ Дескрипторы контролов ------------
HWND hWndBtn1;
#define IDC_BTN1 0
//------------- КОНЕЦ: IDы контролов ------------

//-------------- Перемещение окна ------------------
int width = 300, height = 300;

int xPos, yPos;
int xStep = 10; int yStep = 10;
int DesktopWeidht;
int DesktopHight;

void WindowStep(HWND hWnd)
{
	RECT ClientRect;
	GetWindowRect(hWnd, &ClientRect);

	if ((xPos < DesktopWeidht - (ClientRect.right - ClientRect.left) && ClientRect.top == 0))
	{
		xPos += xStep;
	}
	else
	{
		if ((yPos < DesktopHight - (ClientRect.bottom - ClientRect.top)) && ClientRect.left != 0)
			yPos += yStep;
		else
		{
			if ((xPos > 0 && ClientRect.top == yPos))
				xPos -= xStep;
			else
				yPos -= yStep;
		}
	}
	MoveWindow(hWnd, xPos, yPos, height, width, 1);
}
//------------- Подсчет нажатий ----------------
char szTotal[1024];
int countertLeftButton, counterRightButton, counertCentralButton;



void SetTitle(void) {
	wsprintf(szTotal, _T("Левая = %d, Правая = %d, Центральная= %d"), countertLeftButton, counterRightButton, counertCentralButton);
}

//-------------- Цифровые часы -------------------

void showClock(HDC hdc) {

	char buffer[80];
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	char* format = "%H:%M:%S";
	strftime(buffer, 80, format, timeinfo);
	TextOut(hdc, 2, 80, buffer, 8);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc = GetDC(HWND_DESKTOP);
	static INT timer = 0;

	switch (message)
	{
		//---------- Часы ----------
	case WM_CREATE:
		xPos = yPos = 0;
		DesktopWeidht = GetDeviceCaps(hdc, HORZRES);
		DesktopHight = GetDeviceCaps(hdc, VERTRES);
		ReleaseDC(hWnd, hdc);
		SetTimer(hWnd, 0, 1000, 0);
		break;
	case WM_KEYDOWN:
	{
		switch (LOWORD(wParam))
		{
		case VK_RETURN:
		{
			MoveWindow(hWnd, 0, 0, width, height, 1);
			SetTimer(hWnd, 1, 5, NULL);
			break; }
		case VK_ESCAPE:
		{
			KillTimer(hWnd, 1);
			MoveWindow(hWnd, xPos, yPos, height, width, 1);
			break; }
		}
		break; }
	case WM_TIMER: 
			InvalidateRect(hWnd, 0, TRUE);
			WindowStep(hWnd);
			break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		showClock(hdc);
		EndPaint(hWnd, &ps);
		break;

		//--------- Нажатия кнопок -----
	case WM_LBUTTONDOWN:
		countertLeftButton++;
		SetTitle();
		SetWindowText(hWnd, szTotal);
		break;
	case WM_RBUTTONDOWN:
		counterRightButton++;
		SetTitle();
		SetWindowText(hWnd, szTotal);
		break;
	case WM_MBUTTONDOWN:
		counertCentralButton++;
		SetTitle();
		SetWindowText(hWnd, szTotal);
		break;

		//--------- кнопка Выходл ----------
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDC_BTN1:
			if (MessageBox(hWnd, _T("Действительно выйти ?"), _T("Выход"), MB_OKCANCEL + MB_ICONWARNING) == IDOK)
			{
				DestroyWindow(hWnd);
			}

		}
		break;
		//------ КОНЕЦ: События контролов -------
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

HWND CreateWin(HINSTANCE hInst, int nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("MainWndClass"); //класс окна
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);

	int FrameTitleH = GetSystemMetrics(SM_CYCAPTION);
	int FrameBorderW = GetSystemMetrics(SM_CXSIZEFRAME);

	//---------------- Создание окна ----------------
	HWND hWnd = CreateWindowEx(
		0,
		_T("MainWndClass"),
		_T("Главное мать его окно"),
		WS_VISIBLE | WS_SYSMENU | WS_VISIBLE,
		CW_USEDEFAULT,  // х-координата 
		CW_USEDEFAULT,  // y-координата 
		CW_USEDEFAULT,  
		CW_USEDEFAULT,   
		NULL,      
		NULL,    
		hInst,
		NULL);
	//------------ КОНЕЦ: Создание окна ------------

	//------------- Создание элементов -------------
	if (hWnd != NULL)
	{
		hWndBtn1 = CreateWindowEx(
			0,
			_T("Button"),
			_T("Выход"),
			WS_VISIBLE | WS_CHILD,
			150, 150,
			50, 19,
			hWnd,
			(HMENU)IDC_BTN1,
			hInst,
			0
		);

	}
	//---------- КОНЕЦ: Создание элементов ---------

	return hWnd;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{

	HWND hWnd = CreateWin(hInst, nCmdShow);
	if (hWnd == NULL)
	{
		MessageBox(0, _T("Не удается создать окно."), 0, MB_ICONERROR);
		return 1;
	}
	else
	{
		ShowWindow(hWnd, nCmdShow);
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
