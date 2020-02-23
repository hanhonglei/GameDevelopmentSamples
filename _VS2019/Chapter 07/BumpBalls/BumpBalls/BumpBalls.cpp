// BumpBalls.cpp : 定义应用程序的入口点。
/************************************************************************/
/*	程序目的：C语言中的指针和数组
    程序功能：弹弹球。玩家通过空格在屏幕中心生成随机方向和速度的小球，生成的小球会在窗口范围内发生碰撞
        使用定时器方式来得到动画效果，每隔一定时间触发一次定时器函数，然后更新游戏中的物体状态，以此产生动画效果
    待读者改进:在碰撞检测和碰撞反应上使用更加稳定的策略，解决小球之间粘连等问题
    编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "framework.h"
#include "BumpBalls.h"
#include <math.h>
#include <time.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

#define BALLS_NUM 64							// 最多的小球数量
#define MAX_V 4									// 小球的最大速度
int ballsX[BALLS_NUM], ballsY[BALLS_NUM];		// 数组，保存每个小球的位置
int ballsVX[BALLS_NUM], ballsVY[BALLS_NUM];		// 数组，保存每个小球的速度
COLORREF ballsC[BALLS_NUM];						// 数组，保存每个小球的颜色
int nBalls = 0;									// 当前的小球数量
int radius = 20;								// 小球半径
int timeStep = 50;								// 定时器时间间隔
int wndWidth = 0;								// 窗口尺寸
int wndHeight = 0;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 在窗口中心随机产生一个小球的函数
// @n 当前小球的数量,利用指针方式传递参数,函数内可以对指针指向的变量进行修改
int GenerateBall(int* n)
{
    if (*n >= BALLS_NUM)			// 小球数量过多，则不生成							
        return 0;
    ballsX[*n] = wndWidth / 2;		// 中心位置
    ballsY[*n] = wndHeight / 2;
    ballsVX[*n] = MAX_V - 2 * (rand() % (MAX_V + 1));		// 随机速度
    ballsVY[*n] = MAX_V - 2 * (rand() % (MAX_V + 1));
    ballsC[*n] = RGB(rand() % 256, rand() % 256, rand() % 256);// 随机颜色
    (*n)++;							// 修改参数值,小球数量加1
    return 1;
}
// 绘制小球的函数
// @hdc 绘制设备句柄
// @n 当前的小球数量
// @r 小球半径
// @X Y 小球的位置数组
// @C 小球的颜色数组
void DrawBalls(HDC hdc, int n, int r, int X[], int Y[], COLORREF C[])
{
    HBRUSH brush;
    for (int i = 0; i < n; i++)
    {
        brush = CreateSolidBrush(C[i]);		// 使用当前颜色的笔刷绘制小球
        SelectObject(hdc, brush);
        Ellipse(hdc, X[i] - r, Y[i] - r, X[i] + r, Y[i] + r);
        DeleteObject(brush);
    }
}
// 当两个小球发生碰撞后的反应,计算碰撞后的速度
// @v1 v2 待计算的两个小球的速度
// @u 两个小球的连线向量
int Response(int v1[2], int v2[2], int u[2])
{
    if (u[0] * u[0] + u[1] * u[1] == 0)					// 二者重叠的话，暂时不进行碰撞
        return 0;
    int tmp, v11[2], v12[2], v21[2], v22[2];			// 保存球心连线上和垂直于连线的速度分量
    v11[0] = (v1[0] * u[0] + v1[1] * u[1]) * u[0] / (u[0] * u[0] + u[1] * u[1]);
    v11[1] = (v1[0] * u[0] + v1[1] * u[1]) * u[1] / (u[0] * u[0] + u[1] * u[1]);
    v12[0] = v1[0] - v11[0];
    v12[1] = v1[1] - v11[1];
    v21[0] = (v2[0] * u[0] + v2[1] * u[1]) * u[0] / (u[0] * u[0] + u[1] * u[1]);
    v21[1] = (v2[0] * u[0] + v2[1] * u[1]) * u[1] / (u[0] * u[0] + u[1] * u[1]);
    v22[0] = v2[0] - v21[0];
    v22[1] = v2[1] - v21[1];
    // 在球心连线上速度交换
    tmp = v11[0];
    v11[0] = v21[0];
    v21[0] = tmp;
    tmp = v11[1];
    v11[1] = v21[1];
    v21[1] = tmp;
    // 得到新的速度
    v1[0] = v11[0] + v12[0];
    v1[1] = v11[1] + v12[1];
    v2[0] = v21[0] + v22[0];
    v2[1] = v21[1] + v22[1];
    return 1;
}
// 每一帧，按照小球所处的碰撞状态修改小球速度，并更新小球位置
// @n 小球数量
// @r 小球半径
// @X Y 小球当前位置数组
// @VX VY 小球当前速度数组
// @elapseTime 两帧之间的时间间隔
void UpdateBalls(int n, int r, int X[], int Y[], int VX[], int VY[], int elapseTime)
{
    if (n > BALLS_NUM)
        return;
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            int v1xy[2], v2xy[2], u[2];			// 两个小球的初速度和碰撞方向向量
            // 1 小球是否发生相互碰撞
            // 目前的碰撞检测和反应都只是最简单的计算方法,会出现以下的问题:
            // a.小球之间可能出现粘连
            // b.小球速度过快时,可能检测碰撞时已经错过了
            int dist2 = (X[i] - X[j]) * (X[i] - X[j]) + (Y[i] - Y[j]) * (Y[i] - Y[j]);
            if (dist2 <= 4 * r * r)
            {
                u[0] = X[j] - X[i];
                u[1] = Y[j] - Y[i];
                v1xy[0] = VX[i];
                v1xy[1] = VY[i];
                v2xy[0] = VX[j];
                v2xy[1] = VY[j];
                // 如果小球相碰，则修改小球的速度
                if (Response(v1xy, v2xy, u))
                {
                    VX[i] = v1xy[0];			// 碰撞后两小球的速度的改变
                    VY[i] = v1xy[1];
                    VX[j] = v2xy[0];
                    VY[j] = v2xy[1];
                }
            }
        }
    }
    // 2 处理小球和屏幕边界的碰撞
    for (int i = 0; i < n; i++)
    {
        if ((X[i] - r) <= 0)					// 左边界
            VX[i] = abs(VX[i]);
        else if ((X[i] + r) >= wndWidth)		// 右边界
            VX[i] = -abs(VX[i]);
        if ((Y[i] + r) >= wndHeight)		// 下边界
            VY[i] = -abs(VY[i]);
        else if ((Y[i] - r) <= 0)			// 上边界
            VY[i] = abs(VY[i]);
    }
    // 按照速度更新小球的位置,以便产生动画
    for (int i = 0; i < n; i++)
    {
        X[i] += VX[i] * elapseTime;
        Y[i] += VY[i] * elapseTime;
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BUMPBALLS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BUMPBALLS));

    MSG msg;

    srand((unsigned int)time((NULL)));	// 随机种子
    GenerateBall(&nBalls);	// 随机产生一个小球

    // 主消息循环:
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
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUMPBALLS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BUMPBALLS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
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
    case WM_CREATE:			// 程序启动后
        SetTimer(hWnd, 1, timeStep, NULL);	// 开始设置一个ID为1的定时器,每timeStep毫秒触发一个定时器消息
        break;
    case WM_TIMER:			// 定时器响应消息
        if (wParam == 1)	// 如果是感兴趣的定时器(ID为1),则进行游戏更新
        {
            UpdateBalls(nBalls, radius, ballsX, ballsY, ballsVX, ballsVY, timeStep / 10);
            InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
        }
        break;
    case WM_SIZE:			// 获取窗口的尺寸
        wndWidth = LOWORD(lParam);
        wndHeight = HIWORD(lParam);
        break;
    case WM_KEYDOWN:
        if (wParam == ' ')	// 按下空格
        {
            GenerateBall(&nBalls);				// 生成一个小球
            InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
        }
        break;
    case WM_ERASEBKGND:		// 截获擦除背景消息,不擦除背景,避免闪烁
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
        // 以下的步骤是为了避免产生屏幕闪烁,将画面首先绘制到内存中,然后一次性拷贝到屏幕上
        // 创建内存HDC
            HDC memHDC = CreateCompatibleDC(hdc);

            //获取客户区大小
            RECT rectClient;
            GetClientRect(hWnd, &rectClient);

            //创建位图
            HBITMAP bmpBuff = CreateCompatibleBitmap(hdc, wndWidth, wndHeight);
            HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
            PatBlt(memHDC, 0, 0, wndWidth, wndHeight, WHITENESS);	// 设置背景为白色

            // 进行真正的绘制
            DrawBalls(memHDC, nBalls, radius, ballsX, ballsY, ballsC);

            //拷贝内存HDC内容到实际HDC
            BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, wndWidth,
                wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

            //内存回收
            SelectObject(memHDC, pOldBMP);
            DeleteObject(bmpBuff);
            DeleteDC(memHDC);

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

// “关于”框的消息处理程序。
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
