// Expression.cpp : Defines the entry point for the application.
//
/************************************************************************/
/*	程序目的：C语言中的表达式和语句
	程序功能：计算器。玩家输入四则运算，计算机计算结果并输出。
	待读者改进:实现更加复杂的数学计算,比如(9+22.5)*30/2.3=?
	编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "stdafx.h"
#include "Expression.h"
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

float x = 0, y = 0;			// 参与运算的两个数字
int num = 0;				// 当前用户输入的个数
char oper;					// 运算符
char input[64];				// 临时保存用户的输入信息
float result;				// 保存计算结果
int bResult = 0;			// 是否求得结果
WCHAR str[64];				// 用于输出显示的字符串

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXPRESSION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXPRESSION));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXPRESSION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXPRESSION);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
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
		break;
	case WM_CHAR:												// 玩家输入
		InvalidateRect(hWnd, NULL, TRUE);						// 重绘屏幕消息
		if (wParam == VK_RETURN)								// 如果按下回车键,则开始新一轮计算
		{
			num = 0;
			bResult = 0;
			memset(input, '\0', 64*sizeof(char));
		}
		else if (bResult)										// 如果已经得到了计算结果，则不接收玩家的其他输入，直接跳出这个消息
			break;
		if(wParam == '.' || (wParam >= '0' && wParam <= '9'))	// 如果玩家输入的不是数字,则不记录.逻辑表达式
		{
			input[num++] = wParam;								// 赋值语句
		}
		else if (wParam == '+' || wParam == '-' || wParam == '*' || wParam == '/')	// 记录运算符
		{
			input[num++] = ' ';
			input[num++] = wParam;
			input[num++] = ' ';
		}
		else if (wParam == '=')									// 玩家输入等号,进行运算
		{
			input[num++] = ' ';
			// 修改为安全模式的scanf
			sscanf_s(input, "%f %c %f", &x, &oper, 1, &y);		// 获取参与运算的数字及运算符
			if (oper == '+')									// 依据运算符类型进行不同运算
				result = x + y;									// 算术表达式
			else if( oper == '-')
				result = x - y;
			else if(oper == '*')
				result = x * y;
			else if(oper == '/')
				result = x / y;

			input[num++] = '=';		
			bResult = 1;										// 将计算结果标志设为true
		}
		break; 
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		TextOut(hdc, 0, 0, L"请输入计算式", 6);	
		MultiByteToWideChar(CP_THREAD_ACP,MB_USEGLYPHCHARS,input,64,str,64);   // 将用户的输入转换为宽字符串,以便输出
		TextOut(hdc, 0, 20, str, wcslen(str));					// 显示用户的输入
		if (bResult)											// 如果有计算结果,则在下一行输出结果
		{
			WCHAR str2[16];
			swprintf(str2, 16, L"%f", result);
			TextOut(hdc, 0, 40, str2, wcslen(str2));
		}	
		EndPaint(hWnd, &ps);
		break;	
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
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
