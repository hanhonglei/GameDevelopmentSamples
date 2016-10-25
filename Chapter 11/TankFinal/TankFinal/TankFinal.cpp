// TankFinal.cpp : Defines the entry point for the application.
//
/************************************************************************/
/*	程序目的：指针的高级应用，链表、动态分配删除内存
	程序功能：最终版坦克大战。在游戏场景的顶端三个点随机生成两种不同的敌方坦克
		玩家坦克在下方居中。方向键控制坦克前进方向，空格键发射炮弹。
		游戏具备了读取ini文件、txt文件的能力，不在代码中设置游戏参数
		而是通过读取外部文件的方法，这避免了硬编码，易于游戏维护
		策划人员可以在游戏exe生成完毕的情况下，通过调整外部文件的方式来不断调试游戏数值
		同时，游戏中的物体采用图片的方式，并增加了游戏背景和障碍物。
		游戏中的部分物体还实现了动画功能。
		游戏中的敌人、子弹采用带有头结点的单向链表方式进行管理，便于实现增删操作
		将游戏中的不同功能函数进行分类，并放置于不同文件中，易于维护
		主文件不实现具体功能，只进行功能调用
	待读者改进:实现真正的红白机版坦克大战。
		增加地图编辑功能，允许玩家自定义地图
		更进一步，在编辑器中增加其他功能，比如允许玩家自定义敌人坦克属性等
		最终完成类似于游戏引擎的游戏编辑器功能		
	编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "stdafx.h"
#include "TankFinal.h"
#include <time.h>
#include "Init.h"
#include "GameProc.h"
#include "Entity.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int reset_time = 0;								// 标识是否对计时器进行重置
	
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_TANKFINAL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TANKFINAL));

	srand(time(NULL));			// 随机种子
	ChangeLevel(OPEN);			// 首先进入游戏欢迎界面

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TANKFINAL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;/*MAKEINTRESOURCE(IDC_TANKFINAL);*/	// 取消菜单
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
   SetWindowLong(hWnd, GWL_STYLE,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);	// 不允许改变窗口大小
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
	case WM_CREATE:			// 程序启动后,开始设置3个定时器
		SetTimer(hWnd,1,GetTimeStep(),NULL);
		SetTimer(hWnd,2,GetBirthTime(),NULL);
		SetTimer(hWnd,3,GetAnimStep(),NULL);
		break;
	case WM_TIMER:			// 定时器响应
		InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
		switch (wParam) 
		{ 
		case 1:				// 1号定时器用于更新游戏内容
			Update(GetTimeStep()/10);
			if (reset_time)	// 重新载入关卡之后，按照新的参数设置定时器
			{
				reset_time = 0;
				KillTimer(hWnd,1);		// 程序退出时，将定时器删除
				KillTimer(hWnd,2);
				KillTimer(hWnd,3);
				SetTimer(hWnd,1,GetTimeStep(),NULL);
				SetTimer(hWnd,2,GetBirthTime(),NULL);
				SetTimer(hWnd,3,GetAnimStep(),NULL);
			}
			break ; 
		case 2:				// 2号定时器用于生成敌人坦克
			EnemyBirth();
			break ; 
		case 3:				// 3号定时器用于更新动画帧
			NextFrame(GetAnimStep());
			break ; 
		} 
		break;
	case WM_SIZE:			// 获取窗口的尺寸
		ChangeWndSize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:		// 玩家按下按键消息交由GameProc文件中的特定函数处理
		InvalidateRect(hWnd, NULL, TRUE);
		EnterKey(wParam);
		break;
	case WM_ERASEBKGND:		// 不擦除背景,避免闪烁
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			Draw(hdc, hWnd);		// 进行游戏绘制
			EndPaint(hWnd, &ps);
			break;
		}
	case WM_DESTROY:
		KillTimer(hWnd,1);		// 程序退出时，将定时器删除
		KillTimer(hWnd,2);
		KillTimer(hWnd,3);
		Destroy();
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
