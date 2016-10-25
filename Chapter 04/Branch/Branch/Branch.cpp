// Branch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Branch.h"
#include <time.h>
/************************************************************************/
/*	程序目的：C语言中的分支结构
	程序功能：猜数字。计算机随机生成4个不重复的数字，让玩家猜测，并给出猜测的提示结果，以便让玩家进一步猜测。
		From: http://zh.wikipedia.org/wiki/%E7%8C%9C%E6%95%B0%E5%AD%97
		游戏规则
		通常由两个人玩，一方出数字，一方猜。出数字的人要想好一个没有重复数字的4位数，不能让猜的人知道。猜的人就可以开始猜。每猜一个数字，
		出数者就要根据这个数字给出几A几B，其中A前面的数字表示位置正确的数的个数，而B前的数字表示数字正确而位置不对的数的个数。
		如正确答案为 5234，而猜的人猜 5346，则是 1A2B，其中有一个5的位置对了，记为1A，而3和4这两个数字对了，而位置没对，因此记为 2B，合起来就是 1A2B。
		接着猜的人再根据出题者的几A几B继续猜，直到猜中（即 4A0B）为止。
		猜数字游戏的一种变体允许重复的数码。这种规则的游戏被称为 Mastermind。其规则大致为：
		除了上面的规则外，如果有出现重复的数字，则重复的数字每个也只能算一次，且以最优的结果为准。例如，如正确答案为5543，猜的人猜5255，
		则在这里不能认为猜测的第一个5对正确答案第二个，根据最优结果为准的原理和每个数字只能有一次的规则，两个比较后应该为1A1B，第一个5位子正确，
		记为1A；猜测数字中的第三个5或第四个5和答案的第二个5匹配，只能记为1B。当然，如果有猜5267中的第一个5不能与答案中的第二个5匹配，因此只能记作1A0B。
	待读者改进:修改为支持重复数字的模式
	编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

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

char num[4];				// 保存电脑生成的待猜测数字
char guess[4];				// 玩家猜测的4个数字
int nGuess = 0;				// 玩家已经输入的数字个数
WCHAR str[16];				// 保存输出字符串
int x = 0, y = 0;			// 数字和位置都正确的个数,以及数字正确但位置不正确的个数

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
	LoadString(hInstance, IDC_BRANCH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BRANCH));

	srand(time(NULL));				// 随机种子
	for (int i = 0; i < 4; i++)		// 随机给出4个不重复的数
	{
		num [i] = '0' + rand()%10;
		for (int j = 0; j < i; j++)
		{
			if (num[i] == num[j])	// 保证不出现重复元素
			{
				i--;
				break;
			}
		}
	}

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRANCH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BRANCH);
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
	case WM_KEYDOWN:  
		InvalidateRect(hWnd, NULL, TRUE);	// 重绘屏幕消息
		if(wParam < '0' || wParam > '9')	// 如果玩家输入的不是数字,则不记录
		{
			if (wParam == VK_RETURN)		// 如果玩家输入回车键,则表示重新猜测
			{
				nGuess = 0;
				x = y = 0;
			}
			else if (wParam == ' ')			// 如果玩家输入空格键,则电脑重新生成待猜测的数字
			{
				for (int i = 0; i < 4; i++)
				{
					num [i] = '0' + rand()%10;
					for (int j = 0; j < i; j++)
					{
						if (num[i] == num[j])	// 保证不出现重复元素
						{
							i--;
							break;
						}
					}
				}
				nGuess = 0;
				x = y = 0;
			}
			break;
		}
		if (nGuess < 4)						// 玩家输入猜测的数字
		{
			guess[nGuess++] = wParam;
		}
		if (nGuess == 4)					// 如果已经输入了4个数字,表示猜测完毕，判断结果
		{
			x = y = 0;
			for(int i = 0;i < 4; i++)
				for(int j = 0; j < 4; j++){
					if(num[i] == guess[i]){
						x++;
						break;
					}
					if(guess[j] == num[i])
						y++;
				}
		}

		break; 
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		TextOut(hdc, 0, 0, L"请猜4个数字", 6);
		for (int i = 0; i < nGuess; i++)	// 输出玩家已经猜测的数字
		{
			swprintf(str, L"%c", guess[i]);
			TextOut(hdc, i*20, 20, str, wcslen(str));
		}
		if(nGuess == 4)						// 如果玩家已经输入完毕,则输出猜测结果
		{
			if(x==4){				
				TextOut(hdc, 0, 20, L"猜对了！", 4);
				break;
			}
			else{
				swprintf(str, L"结果：%dA%dB", x, y);
				TextOut(hdc, 80, 20, str, wcslen(str));
			}
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
