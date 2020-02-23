// Bricks.cpp : 定义应用程序的入口点。
/************************************************************************/
/*	程序目的：C语言中的函数
    程序功能：吃砖块。玩家控制一个砖块在屏幕上移动,系统在随机位置生成不同颜色的砖块
        要求玩家控制的砖块尽快移动到系统生成的砖块处,得分.
    待读者改进:修改为贪食蛇的操作模式
    编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/


#include "framework.h"
#include "Bricks.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 全局变量
int xP, yP, xE, yE;	                // 玩家和敌人的位置
const int size = 20;				// 砖块大小
int score = 0;						// 记录玩家得分

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//////////////////////////////////////////////////////////////////////////
// 用户自定义的函数
// 在窗口范围内随机生成一个位置
int RandPos()
{
    return rand() % 501;
}
// 在指定位置绘制一个方块
// @hdc 绘制设备句柄
// @x y 绘制位置
// @s 方块大小
// @bPlayer 标记是否是玩家方块
void DrawBrick(HDC hdc, int x, int y, int s, int bPlayer)
{
    HBRUSH brush;
    brush = bPlayer ? CreateSolidBrush(RGB(255, 0, 0)) : CreateSolidBrush(RGB(0, 255, 0));
    //SelectObject(hdc, brush);
    //Rectangle(hdc,x-size/2,y-size/2,x+size/2,y+size/2);
    RECT rc;
    rc.top = y - s / 2;
    rc.left = x - s / 2;
    rc.bottom = y + s / 2;
    rc.right = x + s / 2;
    FillRect(hdc, &rc, brush);

    DeleteObject(brush);
}
// 判断两个同样大小的方块是否在空间上有重叠
// @参数分别是两个方块的位置以及大小
int IsCollider(int x1, int y1, int x2, int y2, int s)
{
    if (x1 + s < x2 || x1 - s > x2)
        return 0;
    if (y1 + s < y2 || y1 - s > y2)
        return 0;
    return 1;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。
    xP = RandPos(), yP = RandPos(), xE = RandPos(), yE = RandPos();	// 调用随机位置函数,随机生成玩家和敌人的位置

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BRICKS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BRICKS));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BRICKS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BRICKS);
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
    case WM_KEYDOWN:
        InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
        switch (wParam)						// 用户控制主角进行上下左右移动
        {
        case VK_LEFT:
            xP -= xP < 0 ? 0 : 10;
            break;
        case VK_RIGHT:
            xP += xP > 500 ? 0 : 10;
            break;
        case VK_UP:
            yP -= yP < 0 ? 0 : 10;
            break;
        case VK_DOWN:
            yP += yP > 500 ? 0 : 10;
            break;
        }
        if (IsCollider(xP, yP, xE, yE, size))	// 如果主角和敌人位置冲突,则得分,并重新分配敌人位置
        {
            score++;
            xE = RandPos(), yE = RandPos();
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            WCHAR str[16];
            swprintf(str, 16, L"你的得分：%d", score);	// 屏幕输出得分
            TextOut(hdc, 0, 0, str, wcslen(str));
            DrawBrick(hdc, xE, yE, size, 0);		// 绘制敌人和玩家角色
            DrawBrick(hdc, xP, yP, size, 1);
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
