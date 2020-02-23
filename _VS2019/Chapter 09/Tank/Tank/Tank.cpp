// Tank.cpp : 定义应用程序的入口点。
/************************************************************************/
/*	程序目的：用户自定义数据类型
    程序功能：简化版坦克大战。在游戏场景的顶端三个点随机生成两种不同的敌方坦克
        玩家坦克在下方居中。方向键控制坦克前进方向，空格键发射炮弹
    待读者改进:增加胜利画面,并允许玩家重新开始游戏
    编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "framework.h"
#include "Tank.h"
#include <time.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

enum Dir { UP, DOWN, LEFT, RIGHT };	// 枚举类型，运动物体可能的运动方向
typedef struct				// 结构体类型，游戏中的坦克结构体
{
    int x, y;				// 位置
    Dir dir;				// 方向
    int v;					// 速率
    int s;					// 边长，正方形
    int b;					// 是否是子弹
    int p;					// 是否停止,只有玩家才可能停止
    int e;					// 是否是敌人
    COLORREF c;				// 颜色
}Entity;

#define MAX_ENEMY 16		// 最大敌人数目
#define MAX_BULLETS 32		// 最大子弹数目
int nLife = 3;				// 玩家生命
int nScore = 0;				// 玩家得分
int nBullet = 0;			// 玩家打出的子弹数量
int nEnemyBullet = 0;		// 敌人发出的子弹数量
int nEnemy = 0;				// 当前的敌人数量
int timeStep = 20;			// 定时器时间间隔
int sz = 50;				// 坦克尺寸
int velf = 4;				// 快速坦克速率
int vels = 2;				// 慢速坦克速率
int szb = 20;				// 子弹尺寸
int velb = 6;				// 子弹速率
int enemyFirePer = 300;		// 敌人发射子弹的随机比例
int enemyDir = 200;			// 敌人改变方向的随机比例
int bFire = 0;				// 玩家是否处于射击状态
Entity enemys[MAX_ENEMY];	// 敌人数组
Entity player;				// 玩家
Entity bullets[MAX_BULLETS];// 玩家子弹数组
Entity enemyBullets[MAX_BULLETS];// 敌人子弹数组
int wndWidth = 0;	int wndHeight = 0;  // 窗口尺寸

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// 重置玩家信息,恢复到初始状态
void ResetPlayer()
{
	player.s = sz;
	player.b = 0;
	player.c = RGB(122, 30, 0);
	player.dir = UP;
	player.v = vels;
	player.x = wndWidth / 2;
	player.y = wndHeight - sz;
	player.p = 1;
	player.e = 0;
}

// 游戏初始化
void Init()
{
	for (nEnemy = 0; nEnemy < MAX_ENEMY; nEnemy++)
	{
		enemys[nEnemy].s = sz;
		enemys[nEnemy].b = 0;
		enemys[nEnemy].e = 1;
		enemys[nEnemy].dir = Dir(UP + rand() % 4);		// 随机选择一个前进方向
		enemys[nEnemy].v = rand() % 2 == 0 ? velf : vels;	// 在两种速度之间以50%的几率随机选择一种
		enemys[nEnemy].c = enemys[nEnemy].v == velf ? RGB(0, 122, 122) : RGB(0, 60, 30);	// 两种速度坦克的颜色不一样
		enemys[nEnemy].x = (rand() % 3) * (wndWidth - sz) / 2 + sz / 2;	// 随机出生点
		enemys[nEnemy].y = sz;
		enemys[nEnemy].p = 0;
	}
	ResetPlayer();
}

// 对特定游戏实体依据朝向和速率进行移动
void Move(Entity* ent, int ts)
{
	if (ent->p)
		return;
	switch (ent->dir)
	{
	case UP:
		ent->y -= ent->v * ts;
		break;
	case DOWN:
		ent->y += ent->v * ts;
		break;
	case LEFT:
		ent->x -= ent->v * ts;
		break;
	case RIGHT:
		ent->x += ent->v * ts;
		break;
	}
}

// 特定实体进行射击
void Fire(const Entity* ent)
{
	Entity* pBulletes = (ent->e) ? enemyBullets : bullets;	// 判断是敌人还是玩家发射炮弹
	int nB = (ent->e) ? nEnemyBullet : nBullet;
	if (nB >= MAX_BULLETS)
		return;
	(pBulletes + nB)->s = szb;
	(pBulletes + nB)->b = 1;
	(pBulletes + nB)->e = 0;
	(pBulletes + nB)->c = (ent->e) ? RGB(0, 0, 255) : RGB(255, 0, 0);	// 敌人发出炮弹和玩家发出的不一样
	(pBulletes + nB)->dir = ent->dir;
	(pBulletes + nB)->v = velb;
	(pBulletes + nB)->x = ent->x;
	(pBulletes + nB)->p = 0;
	(pBulletes + nB)->y = ent->y;
	switch (ent->dir)	// 炮弹方向就是此时射击物体的朝向
	{
	case UP:
		(pBulletes + nB)->y -= ent->s;
		break;
	case DOWN:
		(pBulletes + nB)->y += ent->s;
		break;
	case LEFT:
		(pBulletes + nB)->x -= ent->s;
		break;
	case RIGHT:
		(pBulletes + nB)->x += ent->s;
		break;
	}
	if (ent->e)
		nEnemyBullet++;
	else
		nBullet++;
}

// 将特定的实体从数组中删除,后续元素向前移动
void Destroy(Entity ents[], int n, int* num)
{
	memcpy(ents + n, ents + n + 1, sizeof(Entity) * ((*num) - 1 - n));
	(*num)--;
}

// 判断两个实体是否发生碰撞,以正方形之间发生碰撞来判断
int IsCollide(const Entity* ent1, const Entity* ent2)
{

	if (ent1->x + ent1->s / 2 <= ent2->x - ent2->s / 2 || ent1->x - ent1->s / 2 >= ent2->x + ent2->s / 2)
		return 0;
	if (ent1->y + ent1->s / 2 <= ent2->y - ent2->s / 2 || ent1->y - ent1->s / 2 >= ent2->y + ent2->s / 2)
		return 0;
	return 1;
}

// 判断特定实体是否和边界发生碰撞
int WallCollide(Entity* ent)
{
	int bC = 0;
	switch (ent->dir)
	{
	case UP:
		if ((ent->y - ent->s / 2) < 0)			// 上边界
		{
			bC = 1;
			ent->y = ent->s / 2;
		}
		break;
	case DOWN:
		if ((ent->y + ent->s / 2) > wndHeight)	// 下边界
		{
			bC = 1;
			ent->y = wndHeight - ent->s / 2;
		}
		break;
	case LEFT:
		if ((ent->x - ent->s / 2) < 0)				// 左边界
		{
			bC = 1;
			ent->x = ent->s / 2;
		}
		break;
	case RIGHT:
		if ((ent->x + ent->s / 2) > wndWidth)		// 右边界
		{
			bC = 1;
			ent->x = wndWidth - ent->s / 2;
		}
		break;
	}
	if (bC)
	{
		if (ent->e)				// 如果敌人和边界发生碰撞,则随机生成新的运动方向
			ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
		else					// 如果是玩家和边界发生碰撞,则玩家停止
			ent->p = 1;
	}
	return bC;
}

// 更新各种游戏信息,定时器消息中会调用这个函数
void Update(int ts)
{
	// 可移动物体位置进行更新
	Entity* ent = NULL;

	for (int i = 0; i < nEnemy; i++)	// 敌人位置更新
	{
		ent = enemys + i;
		Move(ent, ts);
		if ((rand() % enemyFirePer) == 0)
			Fire(ent);
	}
	for (int i = 0; i < nBullet; i++)	// 玩家炮弹位置进行更行
	{
		ent = bullets + i;
		Move(ent, ts);
	}
	for (int i = 0; i < nEnemyBullet; i++)	// 敌人炮弹位置进行更新
	{
		ent = enemyBullets + i;
		Move(ent, ts);
	}
	Move(&player, ts);					// 玩家位置进行更新

	if (bFire)							// 如果玩家处于射击状态,则发射炮弹
	{
		Fire(&player);
		bFire = 0;
	}

	// 判断子弹是否和敌人碰撞
	for (int i = 0; i < nBullet; i++)
	{
		for (int j = 0; j < nEnemy; j++)
		{
			if (IsCollide(&bullets[i], &enemys[j]))
			{
				Destroy(bullets, i, &nBullet);
				Destroy(enemys, j, &nEnemy);
				nScore++;
				i--;
				j--;
				break;
			}
		}
	}
	// 判断敌人的子弹是否和玩家碰撞
	for (int i = 0; i < nEnemyBullet; i++)
	{
		if (IsCollide(&enemyBullets[i], &player))
		{
			Destroy(enemyBullets, i, &nEnemyBullet);
			ResetPlayer();
			nLife--;
			i--;
			break;
		}
	}
	// 判断敌人是否和玩家碰撞
	for (int i = 0; i < nEnemy; i++)
	{
		if (IsCollide(&player, &enemys[i]))
		{
			ResetPlayer();
			nLife--;
		}
	}
	// 判断各种实体是否和游戏边界发生碰撞
	for (int i = 0; i < nEnemy; i++)	// 敌人
	{
		ent = enemys + i;
		if (!WallCollide(ent))		// 有一定几率改变方向
		{
			if (rand() % enemyDir == 0)
				ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
		}
	}
	for (int i = 0; i < nBullet; i++)	// 玩家炮弹
	{
		ent = bullets + i;
		if (WallCollide(ent))
		{
			Destroy(bullets, i, &nBullet);
			i--;
		}
	}
	for (int i = 0; i < nEnemyBullet; i++)	// 敌人炮弹
	{
		ent = enemyBullets + i;
		if (WallCollide(ent))
		{
			Destroy(enemyBullets, i, &nEnemyBullet);
			i--;
		}
	}
	WallCollide(&player);			// 玩家
}
// 绘制参数指定的游戏实体
void DrawEntity(HDC hdc, const Entity* ent)
{
	HBRUSH brush;
	brush = CreateSolidBrush(ent->c);		// 按照实体指定的颜色创建笔刷
	RECT rc;								// 实体长方形
	rc.top = ent->y - ent->s / 2;
	rc.left = ent->x - ent->s / 2;
	rc.bottom = ent->y + ent->s / 2;
	rc.right = ent->x + ent->s / 2;
	FillRect(hdc, &rc, brush);				// 绘制实体主体
	if (!ent->b)							// 如果这个实体不是子弹,则依据朝向绘制炮筒
	{
		switch (ent->dir)
		{
		case UP:
			rc.bottom = rc.top;
			rc.top = rc.bottom - ent->s / 2;
			rc.left = rc.left + ent->s / 4;
			rc.right = rc.right - ent->s / 4;
			break;
		case DOWN:
			rc.top = rc.bottom;
			rc.bottom = rc.bottom + ent->s / 2;
			rc.left = rc.left + ent->s / 4;
			rc.right = rc.right - ent->s / 4;
			break;
		case LEFT:
			rc.right = rc.left;
			rc.left = rc.left - ent->s / 2;
			rc.bottom = rc.bottom - ent->s / 4;
			rc.top = rc.top + ent->s / 4;
			break;
		case RIGHT:
			rc.left = rc.right;
			rc.right = rc.right + ent->s / 2;
			rc.bottom = rc.bottom - ent->s / 4;
			rc.top = rc.top + ent->s / 4;
			break;
		}
		FillRect(hdc, &rc, brush);
	}
	DeleteObject(brush);			// 将使用完的笔刷删除
}

// 绘制整个游戏场景,在其中调用各自的绘制函数完整绘制
void DrawScene(HDC hdc)
{
	// 绘制游戏提示信息
	HFONT hf;
	WCHAR str[32];
	long lfHeight;
	lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	HFONT hfOld = (HFONT)SelectObject(hdc, hf);
	if (nLife <= 0)							// 如果玩家生命为0,则显示结束画面
	{
		SetTextColor(hdc, RGB(122, 0, 0));
		TextOut(hdc, wndWidth / 2 - 100, wndHeight / 2 - 40, L"Game Over", 9);
		SelectObject(hdc, hfOld);
		return;
	}
	// 显示游戏统计信息
	SetTextColor(hdc, RGB(100, 100, 100));
	wsprintf(str, L"Life:%d Score:%d", nLife, nScore);
	TextOut(hdc, wndWidth / 2 - 100, wndHeight / 2 - 40, str, wcslen(str));

	SelectObject(hdc, hfOld);				// 恢复默认字体
	DeleteObject(hf);

	// 绘制各种游戏实体
	Entity* ent = NULL;
	for (int i = 0; i < nEnemy; i++)		// 敌人
	{
		ent = enemys + i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nBullet; i++)		// 玩家发出的炮弹
	{
		ent = bullets + i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nEnemyBullet; i++)	// 敌人发出的炮弹
	{
		ent = enemyBullets + i;
		DrawEntity(hdc, ent);
	}

	DrawEntity(hdc, &player);				// 玩家
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
    LoadStringW(hInstance, IDC_TANK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TANK));

    MSG msg;

	srand((unsigned int)time(NULL));		// 生成随机种子
	Init();					// 游戏信息初始化


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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TANK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TANK);
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
	case WM_CREATE:			// 程序启动后,开始设置一个定时器
		SetTimer(hWnd, 1, timeStep, NULL);
		break;
	case WM_TIMER:			// 定时器响应
		if (wParam == 1)	// 对游戏进行更新
		{
			if (nLife > 0)
				Update(timeStep / 10);
			InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
		}
		break;
	case WM_SIZE:			// 获取窗口的尺寸
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		break;
	case WM_KEYDOWN:		// 玩家按下键盘按键
	{
		InvalidateRect(hWnd, NULL, TRUE);
		switch (wParam)	// 依据玩家输入的信息调整玩家控制的坦克状态
		{
		case VK_LEFT:
			player.dir = LEFT;
			player.p = 0;
			break;
		case VK_RIGHT:
			player.dir = RIGHT;
			player.p = 0;
			break;
		case VK_UP:
			player.dir = UP;
			player.p = 0;
			break;
		case VK_DOWN:
			player.dir = DOWN;
			player.p = 0;
			break;
		case VK_SPACE:	// 射击
			bFire = 1;;
			break;
		}
		break;
	}
	case WM_ERASEBKGND:		// 不擦除背景,避免闪烁
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
			// 以下的步骤是为了避免产生屏幕闪烁,将画面绘制到内存中,一次性拷贝到屏幕上
			//创建内存HDC
			HDC memHDC = CreateCompatibleDC(hdc);

			//获取客户区大小
			RECT rectClient;
			GetClientRect(hWnd, &rectClient);

			//创建位图
			HBITMAP bmpBuff = CreateCompatibleBitmap(hdc, wndWidth, wndHeight);
			HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
			PatBlt(memHDC, 0, 0, wndWidth, wndHeight, WHITENESS);	// 设置背景为白色

			// 进行真正的绘制
			DrawScene(memHDC);

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
		KillTimer(hWnd, 1);		// 程序退出时，将定时器删除
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
