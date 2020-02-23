// Loop.cpp : 定义应用程序的入口点。
//
/************************************************************************/
/*	程序目的：C语言中的循环结构
    程序功能：分形绘制。曼德博集合（Mandelbrot set，或译为曼德布洛特复数集合）是一种在复平面上组成分形的点的集合
        以数学家本华·曼德博的名字命名。曼德博集合与朱利亚集合有些相似的地方，例如使用相同的复二次多项式来进行迭代。
        From http://rosettacode.org/wiki/Mandelbrot_set
        http://en.wikipedia.org/wiki/Mandelbrot_set
    待读者改进:实现其他分形系统的绘制
    编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/


#include "framework.h"
#include "Loop.h"
#include <math.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

const int iXmax = 600;
const int iYmax = 600;
const double CxMin = -2.5;
const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;
const int IterationMax = 200;
const double EscapeRadius = 2;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_LOOP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LOOP));

    MSG msg;

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOOP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LOOP);
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            int iX, iY;
            double Cx, Cy;
            double PixelWidth = (CxMax - CxMin) / iXmax;
            double PixelHeight;
            PixelHeight = (CyMax - CyMin) / iYmax;
            COLORREF color;
            double Zx, Zy;
            double Zx2, Zy2;
            int Iteration;
            double ER2;
            ER2 = EscapeRadius * EscapeRadius;
            // 依据曼德博集合计算原理,获取每个像素的属性
            for (iY = 0; iY < iYmax; iY++)		// 循环每一行
            {
                Cy = CyMin + iY * PixelHeight;
                if (fabs(Cy) < PixelHeight / 2) Cy = 0.0;
                for (iX = 0; iX < iXmax; iX++)	// 循环每一列
                {
                    Cx = CxMin + iX * PixelWidth;
                    Zx = Zy = Zx2 = Zy2 = 0.0;
                    Iteration = 0;
                    // 针对每个像素进行循环计算
                    while (Iteration < IterationMax && ((Zx2 + Zy2) < ER2))
                    {
                        Zy = 2 * Zx * Zy + Cy;
                        Zx = Zx2 - Zy2 + Cx;
                        Zx2 = Zx * Zx;
                        Zy2 = Zy * Zy;
                        Iteration++;
                    }
                    if (Iteration == IterationMax)
                        color = RGB(0, 0, 0);		// 前景色,黑色
                    else
                        color = RGB(255, 255, 255);	// 背景色,白色

                    SetPixel(hdc, iX, iY, color);
                }
            }
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
