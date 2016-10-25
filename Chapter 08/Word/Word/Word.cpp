// Word.cpp : Defines the entry point for the application.
//
/************************************************************************/
/*	程序目的：字符串
	程序功能：单词英雄。画面顶端会不断落下各种单词,要求玩家在单词落地前输入正确的单词
		如果一个单词落地,则生命减1,生命为0,则游戏结束.正确输入一个单词,得分加1
		得分越高单词下落速度越快,但每10分会重置速度为初始状态
	待读者改进:增加部分匹配的识别度，比如部分匹配的字母用高亮显示
	编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "stdafx.h"
#include "Word.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

#define ALL_WORDS 18
#define SCR_WORDS 4
#define MAX_LEN_WORD 16

// 字符串数组,保存所有可能的字符串,每个字符串是一个英文单词
char *candWords[ALL_WORDS] = {"noun","adjective","dverb","pronoun","conjunction","preposition","number","interjection","phrasal"
	,"contain","embrace","vibrate","innovate","extract","entilate","deprive","whitewash","contrary"};
// 当前屏幕上显示的需要玩家键入的单词在candWords中的下标
int scrWords[SCR_WORDS];
// 每个屏幕单词的位置
int posXWords[SCR_WORDS], posYWords[SCR_WORDS];
// 玩家的输入
char inputs[MAX_LEN_WORD];	int nInput = 0;
// 定时器时间间隔
int timeStep = 200;
// 窗口尺寸
int wndWidth = 0;	int wndHeight = 0;  
// 单词移动速度
int v = 1;
// 玩家得分和生命
int nScore = 0;	int nLife = 3;
// 当前玩家部分命中的单词,使用位操作进行计算
unsigned short tmpHits;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// 随机生成屏幕上指定下标id的单词
void GenerateWord(int id)
{
	int done = 0;
	while(!done)			// 保证生成的单词和屏幕上已有单词不重复
	{
		done = 1;
		int candID = scrWords[id]= rand() % (ALL_WORDS);
		for (int i = 0; i < SCR_WORDS; i++)
		{
			if (i == id)
				continue;
			if (scrWords[i] == candID)
			{
				done = 0;
				break;
			}
		}
	}
	// 重置单词的位置和部分命中标志
	posYWords[id] = 0;
	tmpHits ^= tmpHits & (1<<id);
}
// 初始化各种参数
void Init()
{
	memset(scrWords, -1, SCR_WORDS*sizeof(int));	// 屏幕上单词的下标
	memset(posYWords, 0, SCR_WORDS*sizeof(int));	// 屏幕上单词的纵坐标
	memset(inputs, '\0', SCR_WORDS*sizeof(char));	// 当前输入的部分字符
	tmpHits = 0;									// 没有命中单词
	for(int i = 0; i < SCR_WORDS; i++)				// 为屏幕上的单词赋予横坐标
	{
		posXWords[i] = i*wndWidth / SCR_WORDS;
		GenerateWord(i);
	}
	nLife = 3;										// 玩家生命初始为3,得分为0
	nScore = 0;
}
// 处理玩家输入的字母,可能是部分命中,可能全部命中,全部命中要得分,并重置所命中的单词
// @c 当前输入的字母
// @n 当前已经输入完成的字母数,使用指针方式,在函数内对外部变量进行修改
// @ips 保存玩家输入的字母数组
// @返回值 击中的单词下标
int InputChar(char c, int *n, char ips[])
{
	ips[*n] = c;
	int done = 0;
	int hitID = -1;
	unsigned short th = 0;
	for (int i = 0; i < SCR_WORDS; i++)	// 对于屏幕上的所有单词都进行比较
	{
		if(strnicmp(ips, candWords[scrWords[i]], strlen(ips)) == 0)	// 如果部分匹配的话,标记匹配的单词下标(使用位运算)
		{
			done = 1;
			th |= (1<<i);
			if(stricmp(ips, candWords[scrWords[i]]) == 0)			// 如果全部匹配,则这个单词被击中,得分
			{
				nScore++;
				hitID = i;
				v++;
				v = (nScore%10==0)?1:v;								// 每得10分,则重置单词速度
				break;
			}
		}
	}
	if (!done)
		ips[*n] = '\0';
	else
	{
		(*n)++;
		tmpHits = th;
	}
	return hitID;
}
// 重置玩家输入的字符串
void ResetInputs()
{
	nInput = 0;
	memset(inputs, '\0', MAX_LEN_WORD*sizeof(char));
	tmpHits = 0;
}
// 每帧进行更新,依据速度更新单词位置
// @ts 两帧之间时间间隔
void Update(int ts)
{
	for(int i = 0; i < SCR_WORDS; i++)
	{
		if (posYWords[i] >= wndHeight)	// 如果单词已经落地,则减少生命,并新生成一个不重复的单词
		{
			nLife--;
			GenerateWord(i);
			int bReset = 1;
			for (int i = 0; i < SCR_WORDS; i++)
			{
				if (tmpHits & (1<<i))
				{
					bReset = 0;
					break;
				}
			}
			if (bReset)
				ResetInputs();

		}
		else
			posYWords[i] += v * ts;		// 更新屏幕单词位置
	}
}
// 打印,包括玩家信息以及待输入的玩家信息
void PrintWords(HDC hdc)
{
	HFONT hf;
	WCHAR str[MAX_LEN_WORD];
	// 1 用红色字体,在屏幕中心打印玩家信息,如果玩家生命为0,则打印结束信息
	SetTextColor(hdc, RGB(255, 0, 0));
	hf =						//创建逻辑字体
		CreateFont(  
		56,						//字体高度(旋转后的字体宽度)=56    
		20,						//字体宽度(旋转后的字体高度)=20  
		0,						//字体显示角度=0°  
		0,						//nOrientation=0  
		10,						//字体磅数=10 
		FALSE,					//非斜体
		FALSE,					//无下划线
		FALSE,					//无删除线
		DEFAULT_CHARSET,         //使用缺省字符集
		OUT_DEFAULT_PRECIS,		//缺省输出精度
		CLIP_DEFAULT_PRECIS,	//缺省裁减精度
		DEFAULT_QUALITY,		//nQuality=缺省值
		DEFAULT_PITCH,			//nPitchAndFamily=缺省值
		L"@system");			//字体名=@system 
	HFONT hfOld = (HFONT)SelectObject(hdc, hf);
	if (nLife > 0)
	{
		MultiByteToWideChar(CP_THREAD_ACP,MB_USEGLYPHCHARS,inputs,strlen(inputs)+1,str,MAX_LEN_WORD);   // 将用户的输入复制到字符串中
		TextOut(hdc, wndWidth/2-100, wndHeight/2,str,wcslen(str));

		wsprintf(str, L"Life:%d Score:%d", nLife, nScore);	
		TextOut(hdc, wndWidth/2-100, wndHeight/2-40,str,wcslen(str));
	}
	else
	{
		TextOut(hdc, wndWidth/2-100, wndHeight/2-40,L"Game Over",9);
		SelectObject(hdc, hfOld);
		return;
	}	

	// 2 以另外一种字体打印屏幕上待输入的单词
	long lfHeight;
	lfHeight = -MulDiv(50, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	DeleteObject(hf);
	hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");

	SelectObject(hdc, hf);
	for(int i = 0; i < SCR_WORDS; i++)
	{
		if (tmpHits & (1<<i))		// 如果当前单词部分命中,则以特殊颜色显示
			SetTextColor(hdc, RGB(0, 125, 125));
		else
			SetTextColor(hdc, RGB(0, 0, 0));
		MultiByteToWideChar(CP_THREAD_ACP,MB_USEGLYPHCHARS,candWords[scrWords[i]],strlen(candWords[scrWords[i]])+1,str,MAX_LEN_WORD);   // 将用户的输入复制到字符串中
		TextOut(hdc, posXWords[i], posYWords[i],str,wcslen(str));
	}
	SelectObject(hdc, hfOld);		// 恢复默认字体
	DeleteObject(hf);
}

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
	LoadString(hInstance, IDC_WORD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WORD));

	srand(time(NULL));	// 随机种子
	Init();				// 初始化各种参数

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WORD);
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
	case WM_CREATE:			// 程序启动后,开始设置一个定时器
		SetTimer(hWnd,1,timeStep,NULL);
		break;
	case WM_TIMER:			// 定时器响应
		if (wParam == 1)
		{
			Update(timeStep/100);				// 更新游戏内容
			InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
		}
		break;
	case WM_SIZE:			// 窗口缩放消息，获取窗口的尺寸
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		break;
	case WM_CHAR: 
		{
			InvalidateRect(hWnd, NULL, TRUE);		// 重绘屏幕
			if ((wParam >= 'A' && wParam <= 'Z')||	// 如果玩家输入字母,则处理玩家的输入			
				(wParam >= 'a' && wParam <= 'z'))
			{
				int hit = InputChar(wParam, &nInput, inputs);	
				if(hit != -1)						// 如果有单词击中,则重新生成新的单词
				{
					GenerateWord(hit);
					ResetInputs();
				}
			}
			else if (wParam == ' ')					// 玩家输入空格,则重置当前输入
				ResetInputs();
			break;
		}
	case WM_ERASEBKGND:		// 不擦除背景,避免闪烁
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			// 以下的步骤是为了避免产生屏幕闪烁,将画面绘制到内存中,一次性拷贝到屏幕上
			//创建内存HDC
			HDC memHDC = CreateCompatibleDC(hdc);

			//获取客户区大小
			RECT rectClient;
			GetClientRect(hWnd, &rectClient);

			//创建位图
			HBITMAP bmpBuff = CreateCompatibleBitmap(hdc,wndWidth,wndHeight);
			HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
			PatBlt(memHDC,0,0,wndWidth,wndHeight,WHITENESS);	// 设置背景为白色

			// 进行真正的绘制
			PrintWords(memHDC);

			//拷贝内存HDC内容到实际HDC
			BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, wndWidth,
				wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

			//内存回收
			SelectObject(memHDC, pOldBMP);
			DeleteObject(bmpBuff);
			DeleteDC(memHDC);

			EndPaint(hWnd, &ps);
			break;
		}
	case WM_DESTROY:
		KillTimer(hWnd,1);		// 程序退出时，将定时器删除
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
