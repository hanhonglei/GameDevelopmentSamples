// Variable.cpp : 定义应用程序的入口点。
/************************************************************************/
/*	程序目的：C语言各种变量的使用
    程序功能：打字母游戏。玩家有3条生命。屏幕不断出现随机的字母要求玩家尽快正确输入。
        如果输入正确，得1分，否则减一条生命。当生命为0时，结束游戏，给出玩家的平均打字速度。
    待读者改进:修改输出文字的大小，并要求用户在规定时间内完成每个字母的输入，否则减少生命
    编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/


#include "framework.h"
#include "Variable.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
// 全局变量
COLORREF color = RGB(255, 0, 0);				// 文字颜色为红色
int health = 3;									// 玩家生命点数
int score = 0;									// 玩家得分
unsigned long long times = 0;					// 游戏运行时间
char c;											// 待输入的字母
WCHAR str[64];									// 输出字符串，使用扩展的字符存储方式，可以用于国际化文字存储中
char hit;										// 当前用户的按键字母

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
    c = 'A' + rand() % 26;		// 随机生成一个新的待输入字母
    times = GetTickCount64();		// 得到程序启动时的时间（毫秒）

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VARIABLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VARIABLE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VARIABLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_VARIABLE);
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
    case WM_LBUTTONDOWN:					// 鼠标左键按下消息
        color = RGB(255, 0, 0);				// 将字体颜色变换为红色
        InvalidateRect(hWnd, NULL, TRUE);	// 发出重绘消息
        break;
    case WM_RBUTTONDOWN:					// 鼠标右键按下消息
        color = RGB(0, 0, 255);				// 将字体颜色变换为蓝色
        InvalidateRect(hWnd, NULL, TRUE);	// 发出重绘消息
        break;
    case WM_KEYDOWN:						// 键盘按下消息
        if (health <= 0)					// 如果生命点耗尽,则不处理键盘消息
            break;
        hit = wParam;						// 得到当前用户的按键
        if (hit == c)						// 如果是正确的按键
        {
            c = 'A' + rand() % 26;			// 随机生成一个新的待输入字母
            score = score + 1;				// 得分加1
        }
        else								// 否则,减少生命点
        {
            health = health - 1;
            if (health <= 0)				// 如果生命点耗尽,则计算总耗时
                times = GetTickCount64() - times;
        }
        InvalidateRect(hWnd, NULL, TRUE);	// 重绘屏幕
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            HFONT Font1;//创建字体
            Font1 = CreateFont(
                20, // 高度
                0, // 宽度     
                0, //出口    
                0, // 定向       
                FW_BOLD, //粗     
                FALSE, //斜体     
                FALSE, // 下划线      
                0, // 删除线        
                ANSI_CHARSET, // 字符集    
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, // 品质      
                DEFAULT_PITCH | FF_SWISS,
                _T("Consolas")); // 字体
            SelectObject(hdc, Font1);

            SetTextColor(hdc, color);			// 设定文字颜色
            TextOut(hdc, 0, 0, L"请输入正确的字母!", 9);
            swprintf(str, 64, L"生命数：%d", health);	// 将待输出的信息格式化存储到字符串中
            TextOut(hdc, 200, 0, str, wcslen(str));	// 输出游戏信息

            swprintf(str, 64, L"得分：%d", score);	// 将待输出的信息格式化存储到字符串中
            TextOut(hdc, 400, 0, str, wcslen(str));	// 输出游戏信息

            if (health <= 0)					// 游戏结束信息
            {
                swprintf(str, 64, L"游戏结束,打字速度: %.2f 个每秒", float(score * 1000) / times);
            }
            else
                swprintf(str, 64, L"%c", c);		// 当前待输入的字符
            TextOut(hdc, 20, 40, str, wcslen(str));

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
