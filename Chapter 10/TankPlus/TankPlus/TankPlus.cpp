// TankPlus.cpp : Defines the entry point for the application.
//
/************************************************************************/
/*	程序目的：文件的使用
	程序功能：改进版坦克大战。在游戏场景的顶端三个点随机生成两种不同的敌方坦克
		玩家坦克在下方居中。方向键控制坦克前进方向，空格键发射炮弹。
		游戏具备了读取ini文件、txt文件的能力，不在代码中设置游戏参数
		而是通过读取外部文件的方法，这避免了硬编码，易于游戏维护
		策划人员可以在游戏exe生成完毕的情况下，通过调整外部文件的方式来不断调试游戏数值
		同时，游戏中的物体采用图片的方式，并增加了游戏背景和障碍物
	待读者改进:增加其他障碍物，并将尽可能多的游戏内容相关参数放置于外部文件中
	编写者：韩红雷（hanhonglei@sina.com）*/
/************************************************************************/

#include "stdafx.h"
#include "TankPlus.h"
#include <time.h>
#include <math.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

enum Dir{UP, DOWN, LEFT, RIGHT, NONE};	// 运动物体可能的运动方向

typedef struct				// 游戏中的实体结构体
{
	int x, y;				// 位置
	Dir dir;				// 方向
	int v;					// 速率
	int s;					// 边长，正方形
	int b;					// 是否是子弹
	int p;					// 是否停止,只有玩家才可能停止
	int e;					// 是否是敌人
	HBITMAP a[4];			// 位图,4个方向
}Entity;

HBITMAP tank1[4],tank2[4],tank3[4],bulletPic[4],grass,background;	// 保存载入的位图句柄
#define MAX_ENEMY 16		// 最大敌人数目
#define MAX_BULLETS 32		// 最大子弹数目
#define MAX_GRASS 64		// 最大的障碍物数量
int nLife = 3;				// 玩家生命
int nScore = 0;				// 玩家得分
int nBullet = 0;			// 玩家打出的子弹数量
int nEnemyBullet = 0;		// 敌人发出的子弹数量
int nEnemy = 0;				// 当前的敌人数量
int nGrass = 0;				// 障碍物数量
int timeStep = 20;			// 定时器时间间隔
int sz = 50;				// 坦克尺寸
int velf = 4;				// 快速坦克速率
int vels = 2;				// 慢速坦克速率
int szb = 20;				// 子弹尺寸
int velb = 6;				// 子弹速率
int enemyFirePer = 300;		// 敌人发射子弹的随机比例
int enemyDirPer = 200;		// 敌人随机改变方向的比例
int bFire = 0;				// 是否处于开火状态

Entity grasses[MAX_GRASS];	// 草地
Entity enemys[MAX_ENEMY];	// 敌人数组
Entity player;				// 玩家
Entity bullets[MAX_BULLETS];// 玩家子弹数组
Entity enemyBullets[MAX_BULLETS];// 敌人子弹数组
int wndWidth = 0;	int wndHeight = 0;  // 窗口尺寸

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// 绘制参数指定的游戏实体
void DrawEntity(HDC hdc, const Entity *ent)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP bmp = ent->a[int(ent->dir)%4];
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, bmp);	// 选定绘制的位图
	BITMAP bm;
	GetObject(bmp, sizeof(bm), &bm);
	SetStretchBltMode(hdc,STRETCH_HALFTONE);				// 设定位图缩放模式为STRETCH_HALFTONE,提高缩放质量
	// 以背景透明的方式绘制位图,白色作为透明色，需要链接 Library Msimg32.lib
	TransparentBlt(hdc,ent->x-ent->s/2, ent->y-ent->s/2, ent->s, ent->s, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight,RGB(255,255,255));
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}
// 绘制整个游戏场景,在其中调用各自的绘制函数完整绘制
void DrawScene(HDC hdc)
{
	// 绘制背景位图
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, background);
	BITMAP bm;
	GetObject(background, sizeof(bm), &bm);
	SetStretchBltMode(hdc,STRETCH_HALFTONE);
	BitBlt(hdc, 0, 0, wndWidth, wndHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	// 打印游戏提示信息
	HFONT hf;
	WCHAR str[32];
	long lfHeight;
	lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	HFONT hfOld = (HFONT)SelectObject(hdc, hf);
	SetBkMode(hdc, TRANSPARENT);	// 背景是透明色
	if (nLife <= 0)					// 如果玩家生命为0,则游戏结束
	{
		SetTextColor(hdc, RGB(122, 0, 0));
		TextOut(hdc, wndWidth/2-100, wndHeight/2-40,L"Game Over",9);
		SelectObject(hdc, hfOld);
		return;
	}	
	SetTextColor(hdc, RGB(100, 100, 100));
	wsprintf(str, L"Life:%d Score:%d", nLife, nScore);	
	TextOut(hdc, wndWidth/2-100, wndHeight/2-40,str,wcslen(str));

	SelectObject(hdc, hfOld);		// 恢复默认字体
	DeleteObject(hf);

	// 绘制游戏实体
	Entity* ent = NULL;
	for (int i = 0; i < nEnemy; i++)	// 敌人
	{
		ent = enemys+i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nBullet; i++)	// 玩家炮弹
	{
		ent = bullets+i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nEnemyBullet; i++)	// 敌人炮弹
	{
		ent = enemyBullets+i;
		DrawEntity(hdc, ent);
	}
	for (int i = 0; i < nGrass; i++)	// 场景中的障碍物
	{
		ent = grasses+i;
		DrawEntity(hdc, ent);
	}

	DrawEntity(hdc, &player);			// 玩家
}
// 重置玩家信息
void ResetPlayer()
{
	player.s = sz;
	player.b = 0;
	memcpy(player.a, tank1, 4*sizeof(Dir));
	player.dir = UP;
	player.v = vels;
	player.x = wndWidth/2;
	player.y = wndHeight-sz;
	player.p = 1;
	player.e = 0;
}
// 通过ini文件读取初始化信息
void ReadIni()
{
	timeStep=GetPrivateProfileInt(L"Global", L"timeStep",timeStep, L"Resources\\Init.ini");
	sz = GetPrivateProfileInt(L"Enemy", L"size",sz, L"Resources\\Init.ini");
	velf = GetPrivateProfileInt(L"Enemy", L"velf",velf, L"Resources\\Init.ini");
	vels = GetPrivateProfileInt(L"Enemy", L"vels",vels, L"Resources\\Init.ini");
	enemyFirePer = GetPrivateProfileInt(L"Enemy", L"firePer",enemyFirePer, L"Resources\\Init.ini");
	enemyDirPer = GetPrivateProfileInt(L"Enemy", L"dirPer",enemyDirPer, L"Resources\\Init.ini");
	nLife = GetPrivateProfileInt(L"Player", L"nLife",nLife, L"Resources\\Init.ini");
	szb = GetPrivateProfileInt(L"Bullet", L"size",szb, L"Resources\\Init.ini");
	velb = GetPrivateProfileInt(L"Bullet", L"vel",velb, L"Resources\\Init.ini");
}
// 读取地图配置文件,标记为1的地方有障碍物
void InitMap()
{
	FILE *f = fopen("Resources\\Map.txt", "r");	// 打开地图配置文件
	if (f == NULL)
		return;
	char line[MAX_GRASS];
	int nLine = 0;
	while(!feof(f))
	{
		fgets(line, MAX_GRASS, f);				// 读取文件中的每一行(以字符串形式)
		for (int i = 0; line[i] != '\0'; i++)	
		{
			if (line[i] == '1')					// 字符1表示此处有障碍物
			{
				if (nGrass > MAX_GRASS)
					break;
				grasses[nGrass].s = sz;
				grasses[nGrass].b = 0;
				grasses[nGrass].e = 0;
				grasses[nGrass].dir = NONE;
				grasses[nGrass].v = 0;
				grasses[nGrass].a[0] = grasses[nGrass].a[1] = grasses[nGrass].a[2] = grasses[nGrass].a[3] = grass;
				grasses[nGrass].x = sz/2+sz*i;
				grasses[nGrass].y = sz/2+sz*nLine;
				grasses[nGrass].p = 0;
				nGrass++;
			}
		}
		nLine++;
	}
	fclose(f);		// 读取完毕关闭文件
}
// 游戏初始化
void Init()
{
	// 首先读取配置文件中的参数填充全局变量
	ReadIni();

	//从硬盘载入图片
	tank1[0] = (HBITMAP)LoadImage( NULL, L"Resources\\TankBlue.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank2[0] = (HBITMAP)LoadImage( NULL, L"Resources\\TankYellow.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank3[0] = (HBITMAP)LoadImage( NULL, L"Resources\\TankLarge.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	bulletPic[0] = (HBITMAP)LoadImage( NULL, L"Resources\\bullet.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank1[1] = (HBITMAP)LoadImage( NULL, L"Resources\\TankBlue1.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank2[1] = (HBITMAP)LoadImage( NULL, L"Resources\\TankYellow1.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank3[1] = (HBITMAP)LoadImage( NULL, L"Resources\\TankLarge1.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	bulletPic[1] = (HBITMAP)LoadImage( NULL, L"Resources\\bullet1.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank1[2] = (HBITMAP)LoadImage( NULL, L"Resources\\TankBlue2.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank2[2] = (HBITMAP)LoadImage( NULL, L"Resources\\TankYellow2.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank3[2] = (HBITMAP)LoadImage( NULL, L"Resources\\TankLarge2.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	bulletPic[2] = (HBITMAP)LoadImage( NULL, L"Resources\\bullet2.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank1[3] = (HBITMAP)LoadImage( NULL, L"Resources\\TankBlue3.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank2[3] = (HBITMAP)LoadImage( NULL, L"Resources\\TankYellow3.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	tank3[3] = (HBITMAP)LoadImage( NULL, L"Resources\\TankLarge3.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	bulletPic[3] = (HBITMAP)LoadImage( NULL, L"Resources\\bullet3.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );
	// 背景图片
	background = (HBITMAP)LoadImage( NULL, L"Resources\\Back.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );

	//从资源文件中载入图片(需要在资源窗口中将位图导入)
	grass=LoadBitmap (hInst,MAKEINTRESOURCE(IDB_GRASS)); 

	// 初始化敌人
	for(nEnemy = 0; nEnemy < MAX_ENEMY; nEnemy++)
	{
		enemys[nEnemy].s = sz;
		enemys[nEnemy].b = 0;
		enemys[nEnemy].e = 1;
		enemys[nEnemy].dir = Dir(UP+rand()%4);
		enemys[nEnemy].v = rand()%2==0?velf:vels;
		enemys[nEnemy].v == velf?	memcpy(enemys[nEnemy].a, tank3, 4*sizeof(Dir)):memcpy(enemys[nEnemy].a, tank2, 4*sizeof(Dir));
		enemys[nEnemy].x = (rand()%3)*(wndWidth-sz)/2 + sz/2;
		enemys[nEnemy].y = sz;
		enemys[nEnemy].p = 0;
	}

	// 初始化玩家
	ResetPlayer();

	// 初始化地图
	InitMap();
}
// 对特定游戏实体进行移动
void Move(Entity *ent, int ts)
{
	if (ent->p)
		return;
	switch(ent->dir)
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
// 特定实体进行射击操作
void Fire(const Entity* ent)
{
	Entity *pBulletes = (ent->e)?enemyBullets:bullets;
	int nB = (ent->e)?nEnemyBullet:nBullet;
	if (nB >= MAX_BULLETS)
		return;
	(pBulletes+nB)->s = szb;
	(pBulletes+nB)->b = 1;
	(pBulletes+nB)->e = 0;
	memcpy((pBulletes+nB)->a, bulletPic, 4*sizeof(Dir));
	(pBulletes+nB)->dir = ent->dir;
	(pBulletes+nB)->v = velb;
	(pBulletes+nB)->x = ent->x;
	(pBulletes+nB)->p = 0;
	(pBulletes+nB)->y = ent->y;
	switch(ent->dir)
	{
	case UP:
		(pBulletes+nB)->y -= ent->s;
		break;
	case DOWN:
		(pBulletes+nB)->y += ent->s;
		break;
	case LEFT:
		(pBulletes+nB)->x -= ent->s;
		break;
	case RIGHT:
		(pBulletes+nB)->x += ent->s;
		break;
	}
	if (ent->e)
		nEnemyBullet++;
	else
		nBullet++;
}
// 将特定的实体从数组中删除,后续元素向前移动
void Destroy(Entity ents[], int n, int *num)
{
	memcpy(ents+n, ents+n+1, sizeof(Entity)*((*num)-1-n));
	(*num)--;
}
// 判断两个实体是否发生碰撞,以正方体之间发生碰撞来判断
int IsCollide(const Entity *ent1, const Entity *ent2)
{

	if (ent1->x+ent1->s/2 <= ent2->x-ent2->s/2 || ent1->x-ent1->s/2 >= ent2->x + ent2->s/2)
		return 0;
	if (ent1->y+ent1->s/2 <= ent2->y-ent2->s/2 || ent1->y-ent1->s/2 >= ent2->y + ent2->s/2)
		return 0;
	return 1;
}
// 判断特定实体是否和边界发生碰撞
int WallCollide(Entity *ent)
{
	int bC = 0;
	switch(ent->dir)
	{
	case UP:
		if ((ent->y - ent->s/2) < 0)			// 上边界
		{
			bC = 1;
			ent->y = ent->s/2;
		}
		break;
	case DOWN:
		if ((ent->y + ent->s/2) > wndHeight)	// 下边界
		{
			bC = 1;
			ent->y = wndHeight - ent->s/2;
		}
		break;
	case LEFT:
		if((ent->x - ent->s/2) < 0)				// 左边界
		{
			bC = 1;
			ent->x = ent->s/2;
		}
		break;
	case RIGHT:
		if((ent->x + ent->s/2) > wndWidth)		// 右边界
		{
			bC = 1;
			ent->x = wndWidth - ent->s/2;
		}
		break;
	}
	if (bC)
	{
		if (ent->e)
			ent->dir = Dir((ent->dir+1+rand()%3)%4);
		else
			ent->p = 1;
	}
	return bC;
}
// 更新各种游戏信息,定时器消息中会调用这个函数
void Update(int ts)
{
	// 各种可移动物体进行位置更新
	Entity* ent = NULL;
	for (int i = 0; i < nEnemy; i++)
	{
		ent = enemys+i;
		Move(ent, ts);
		if ((rand()%enemyFirePer) == 0)
			Fire(ent);
	}
	for (int i = 0; i < nBullet; i++)
	{
		ent = bullets+i;
		Move(ent, ts);
	}
	for (int i = 0; i < nEnemyBullet; i++)
	{
		ent = enemyBullets+i;
		Move(ent, ts);
	}
	Move(&player, ts);

	// 玩家处于开火状态的话,发射子弹
	if (bFire)
	{
		Fire(&player);
		bFire = 0;
	}

	// 判断子弹是否和敌人碰撞
	for(int i = 0; i < nBullet; i++)
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
	for(int i = 0; i < nEnemyBullet; i++)
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
	for (int i = 0; i < nEnemy; i++)
	{
		ent = enemys+i;
		if (!WallCollide(ent))		// 有一定几率改变方向
		{
			int cg = 0;
			for (int j = 0; j < nGrass; j++)	// 判断敌人是否和场景中障碍物发生碰撞
			{
				if (IsCollide(ent, grasses+j))
				{
					cg = 1;

					Move(ent, -ts);
					break;
				}
			}
			if (rand()%enemyDirPer == 0 || cg)
				ent->dir = Dir((ent->dir+1+rand()%3)%4);
		}
	}
	// 判断玩家角色是否和障碍物发生碰撞,如果是,则停止
	for (int i = 0; i < nGrass; i++)
	{
		ent = grasses+i;
		if (IsCollide(ent, &player))
		{
			switch(player.dir)
			{
			case UP:
				player.y = ent->y+ent->s;
				break;
			case DOWN:
				player.y = ent->y-ent->s;
				break;
			case LEFT:
				player.x = ent->x+ent->s;
				break;
			case RIGHT:
				player.x = ent->x-ent->s;
				break;
			}
			player.p = 1;
			break;
		}
	}
	// 判断子弹和边界的碰撞
	for (int i = 0; i < nBullet; i++)
	{
		ent = bullets+i;
		if (WallCollide(ent))
		{
			Destroy(bullets, i, &nBullet);
			i--;
		}
	}
	for (int i = 0; i < nEnemyBullet; i++)
	{
		ent = enemyBullets+i;
		if (WallCollide(ent))
		{
			Destroy(enemyBullets, i, &nEnemyBullet);
			i--;
		}
	}
	// 玩家和边界的碰撞
	WallCollide(&player);
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
	LoadString(hInstance, IDC_TANKPLUS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TANKPLUS));

	srand(time(NULL));			// 随机种子
	Init();						// 初始化游戏信息

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TANKPLUS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_TANKPLUS);		// 去掉菜单栏
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
   SetWindowLong(hWnd, GWL_STYLE,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU);	// 不允许改变窗口大小,以便准确设定场景中的各种物体位置
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
		if (wParam == 1)	// 对游戏进行更新
		{
			if (nLife > 0)
				Update(timeStep/10);
			InvalidateRect(hWnd, NULL, TRUE);	// 让窗口变为无效,从而触发重绘消息
		}
		break;
	case WM_SIZE:			// 获取窗口的尺寸
		wndWidth = LOWORD(lParam);
		wndHeight = HIWORD(lParam);
		break;
	case WM_KEYDOWN: 
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
			case VK_SPACE:
				bFire = 1;
				break;
			}
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
			DrawScene(memHDC);

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
