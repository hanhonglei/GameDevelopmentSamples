/************************************************************************/
/*	该文件主要实现游戏运行过程中的各种功能
	并且要保存各种游戏运行参数*/
/************************************************************************/
#include "stdafx.h"
#include "GameProc.h"
#include "Init.h"

int nScore = 0;				// 玩家得分
int bFire = 0;				// 是否处于开火状态
int wndWidth = 0;	int wndHeight = 0;  // 窗口尺寸
LEVEL curLevel = OPEN;		// 当前所处的关卡

Entity *pHeadBlocks = NULL;	// 障碍物链表
Entity *pHeadEnemys = NULL;	// 敌人链表
Entity *pPlayer = NULL;		// 玩家
Entity *pHeadBullets = NULL;// 玩家子弹链表
Entity *pHeadEnemyBullets = NULL;// 敌人子弹链表

extern int reset_time;		// 是否重新启动定时器

// 依据所处关卡的不同,随机生成一个敌人
void EnemyBirth()
{
	if (curLevel != LEVEL1 && curLevel != LEVEL2)	// 只有在进入真正关卡时,才有必要对游戏内容进行初始化
		return;

	int type = 0;
	switch (curLevel)
	{
	case LEVEL1:
		type =	(rand()%3)== 0 ? 1: 0;	// 三分之二的几率得到慢速的坦克
		break;
	case LEVEL2:
		type =	(rand()%3)== 0 ? 0: 1;	// 三分之二的几率得到快速的坦克
		break;
	}
	// 调用Init中的敌人生成函数,完成对该敌人的初始化
	EnemyBirth(pHeadEnemys, type, wndWidth);
}
// 处理按键消息
void EnterKey(int key)
{
	switch(curLevel)
	{
	case OPEN:					// 开场场景中,只对回车键进行处理
		if (key == VK_RETURN)
			ChangeLevel(LEVEL1);
		break;
	case LEVEL2_OPEN:			
		if (key == VK_RETURN)
			ChangeLevel(LEVEL2);
		break;
	case FAIL:
	case SUCCEED:
		if (key == VK_RETURN)
			ChangeLevel(OPEN);
		break;
	case LEVEL1:
	case LEVEL2:
		switch (key)	// 依据玩家输入的信息调整玩家控制的坦克状态
		{
		case VK_LEFT:
			pPlayer->dir = LEFT;
			pPlayer->p = 0;
			break;
		case VK_RIGHT:
			pPlayer->dir = RIGHT;
			pPlayer->p = 0;
			break;
		case VK_UP:
			pPlayer->dir = UP;
			pPlayer->p = 0;
			break;
		case VK_DOWN:
			pPlayer->dir = DOWN;
			pPlayer->p = 0;
			break;
		case VK_SPACE:
			bFire = 1;
			break;
		}		
		break;
	}
}
// 对各种链表头结点进行动态分配
void NewEntity()
{
	if(pHeadBlocks == NULL)
	{
		pHeadBlocks = (Entity*)malloc(sizeof(Entity));
		pHeadBlocks->next = NULL;
	}
	if(pHeadEnemys == NULL)
	{
		pHeadEnemys = (Entity*)malloc(sizeof(Entity));
		pHeadEnemys->next = NULL;
	}
	if(pPlayer == NULL)
	{
		pPlayer = (Entity*)malloc(sizeof(Entity));
		pPlayer->next = NULL;
	}
	if(pHeadBullets == NULL)
	{
		pHeadBullets = (Entity*)malloc(sizeof(Entity));
		pHeadBullets->next = NULL;
	}
	if(pHeadEnemyBullets == NULL)
	{
		pHeadEnemyBullets = (Entity*)malloc(sizeof(Entity));
		pHeadEnemyBullets->next =NULL;
	}
}
// 窗口大小改变通知函数,文件的全局变量中保存窗口的尺寸
void ChangeWndSize(int w, int h)
{
	wndWidth = w, wndHeight = h;
}
// 转换关卡
void ChangeLevel(LEVEL newL)
{
	curLevel = newL;
	Init(newL);		// 初始化游戏参数
	if (newL != LEVEL1 && newL != LEVEL2)	// 只有在进入真正关卡时,才有必要对游戏内容进行初始化
		return;
	NewEntity();	// 新建各种链表头
	InitEntity(pHeadEnemys, pHeadBlocks, pHeadBullets, pHeadEnemyBullets, pPlayer, newL);	// 对这些链表头进行初始化
	ResetPlayer(pPlayer);	// 重置玩家信息
	nScore = 0;		// 重置游戏统计信息
	reset_time = 1;
}
// 重置玩家信息
void ResetPlayer(Entity *player)
{
	player->p = 1;
	player->dir = UP;
	player->x = wndWidth/2;
	player->y = wndHeight-player->s;
}
// 完成游戏每帧的绘制
void Draw(HDC hdc, HWND hWnd)
{
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
	DrawScene(memHDC, curLevel);

	//拷贝内存HDC内容到实际HDC
	BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top, wndWidth,
		wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);

	//内存回收
	SelectObject(memHDC, pOldBMP);
	DeleteObject(bmpBuff);
	DeleteDC(memHDC);
}
// 依据当前所处的关卡,绘制整个游戏场景,在其中调用各自的绘制函数完整绘制
void DrawScene(HDC hdc, LEVEL curL)
{
	HFONT hf;
	WCHAR str[32];
	SetBkMode(hdc, TRANSPARENT);	// 背景是透明色
	long lfHeight;
	lfHeight = -MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	HFONT hfOld;
	hf = CreateFont(lfHeight, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
	hfOld = (HFONT)SelectObject(hdc, hf);
	SetTextColor(hdc, RGB(122, 0, 0));
	HBITMAP cb = GetLogoPic();
	switch (curL)
	{
	case OPEN:		// 开场阶段,输出提示信息
		cb = GetLogoPic();
		wsprintf(str, L"%s", L"按下回车键开始游戏");	
		break;
	case LEVEL1:	// 游戏进行阶段,完成对游戏场景内容的绘制
	case LEVEL2:
		SetTextColor(hdc, RGB(100, 0, 0));
		lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		DeleteObject(hf);
		hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
		SelectObject(hdc, hf);
		wsprintf(str, L"Life:%d Score:%d", pPlayer->life, nScore);	
		DrawGameScene(hdc);
		break;
	case LEVEL2_OPEN:// 开场阶段,输出提示信息
		cb = GetSuccPic();
		wsprintf(str, L"%s", L"按下回车键开始下一关");	
		break;
	case SUCCEED:	// 输出提示信息
		cb = GetSuccPic();
		wsprintf(str, L"%s", L"胜利！按下回车键重新开始");	
		break;
	case FAIL:		// 输出提示信息
		cb = GetFailPic();
		wsprintf(str, L"%s", L"失败！按下回车键重新开始");	
		break;
	}
	if (curL==OPEN||curL==LEVEL2_OPEN||curL==FAIL||curL==SUCCEED)
	{
		// 绘制背景
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, cb);
		BITMAP bm;
		GetObject(cb, sizeof(bm), &bm);
		SetStretchBltMode(hdc,STRETCH_HALFTONE);
		BitBlt(hdc, wndWidth/2-bm.bmWidth/2, wndHeight/2-bm.bmHeight/2, wndWidth, wndHeight, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem);

	}
	TextOut(hdc, wndWidth/2-100, wndHeight-100,str,wcslen(str));

	SelectObject(hdc, hfOld);		// 恢复默认字体
	DeleteObject(hf);
}
// 完成游戏内容的绘制
void DrawGameScene(HDC hdc)
{
	// 绘制背景
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, GetBackPic());
	BITMAP bm;
	GetObject(GetBackPic(), sizeof(bm), &bm);
	SetStretchBltMode(hdc,STRETCH_HALFTONE);
	BitBlt(hdc, 0, 0, wndWidth, wndHeight, hdcMem, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);

	// 绘制游戏实体
	DrawEntities(hdc, pHeadEnemys);
	DrawEntities(hdc, pHeadBullets);
	DrawEntities(hdc, pHeadEnemyBullets);
	DrawEntities(hdc, pHeadBlocks);
	DrawEntity(hdc, pPlayer);
}
// 对动态分配的内容进行释放,避免内存泄露
void Destroy()
{
	Destroy(pHeadBlocks);
	Destroy(pHeadEnemys);
	Destroy(pHeadBullets);
	Destroy(pHeadEnemyBullets);

	free(pPlayer);
	free(pHeadBlocks);
	free(pHeadEnemys);
	free(pHeadBullets);
	free(pHeadEnemyBullets);

	pHeadBlocks = NULL;		
	pHeadEnemys = NULL;		
	pPlayer = NULL;			
	pHeadBullets = NULL;	
	pHeadEnemyBullets = NULL;
}
// 更新游戏场景中的动画元素
void NextFrame(int ts)
{
	// 调用Entity中的函数,完成各种有动画物体的当前帧更新
	NextFrameEntites(pHeadEnemys, ts);
	NextFrameEntites(pHeadBullets, ts);
	NextFrameEntites(pHeadEnemyBullets, ts);
	NextFrame(pPlayer, ts);
}
// 更新各种游戏信息,定时器会触发这个函数
void Update(int ts)
{
	if (curLevel != LEVEL1 && curLevel != LEVEL2)
		return;

	// 调用Entity中的函数,完成各种可移动物体进行位置更新
	MoveEntities(pHeadEnemys, ts);
	MoveEntities(pHeadBullets, ts);
	MoveEntities(pHeadEnemyBullets, ts);
	Move(pPlayer, ts);

	// 玩家处于开火状态的话,调用Entity中的函数发射子弹
	if (bFire)
	{
		Fire(pPlayer, pHeadBullets);
		bFire = 0;
	}

	Entity *freeP = NULL;
	// 判断子弹是否和敌人碰撞
	for(Entity *ip = pHeadBullets->next; ip != NULL; ip = ip->next)	// 遍历子弹链表
	{
		if (freeP != NULL)				// 如果上一个子弹碰到了敌人,则销毁那个子弹
		{
			Destroy(pHeadBullets, freeP);
			freeP = NULL;
		}
		for (Entity *jp = pHeadEnemys->next; jp != NULL; jp = jp->next)
		{
			if (IsCollide(ip, jp))		// 如果子弹碰到了当前的敌人
			{
				freeP = ip;				// 记录发生碰撞的子弹指针,在下次子弹循环中进行删除
				jp->life--;
				if (jp->life <= 0)
				{
					Destroy(pHeadEnemys, jp);	// 销毁敌人
					nScore++;					// 得分
				}
				break;
			}
		}
	}
	if (freeP != NULL)					// 如果是最后一个子弹碰到了敌人,则销毁
	{
		Destroy(pHeadBullets, freeP);
		freeP = NULL;
	}

	// 判断敌人的子弹是否和玩家碰撞
	for (Entity *p = pHeadEnemyBullets->next; p != NULL; p = p->next)
	{
		if (IsCollide(p, pPlayer))
		{
			Destroy(pHeadEnemyBullets, p);
			ResetPlayer(pPlayer);
			pPlayer->life--;
			break;
		}
	}
	// 判断敌人是否和玩家碰撞
	for (Entity *p = pHeadEnemys->next; p != NULL; p = p->next)
	{
		if (RandomFire())
			Fire(p, pHeadEnemyBullets);


		if (IsCollide(pPlayer,p))
		{
			ResetPlayer(pPlayer);
			pPlayer->life--;
		}
	}
	// 判断各种实体是否和游戏边界发生碰撞
	for (Entity *p = pHeadEnemys->next; p != NULL; p = p->next)
	{
		if (!WallCollide(p, wndWidth, wndHeight))		// 有一定几率改变方向
		{
			int cg = 0;
			for (Entity *q = pHeadBlocks->next; q != NULL; q = q->next)	// 判断敌人是否和场景中障碍物发生碰撞
			{
				if (IsCollide(p, q))
				{
					cg = 1;

					Move(p, -ts);					// 后退一步
					break;
				}
			}
			if (RandomDir() || cg)
				p->dir = Dir((p->dir+1+rand()%3)%4);
		}
		else
			p->dir = Dir((p->dir+1+rand()%3)%4);

	}
	// 判断玩家角色是否和障碍物发生碰撞,如果是,则停止
	for (Entity *p = pHeadBlocks->next; p != NULL; p = p->next)	
	{
		if (IsCollide(p, pPlayer))
		{
			switch(pPlayer->dir)
			{
			case UP:
				pPlayer->y = p->y+(p->s+pPlayer->s)/2;
				break;
			case DOWN:
				pPlayer->y = p->y-(p->s+pPlayer->s)/2;
				break;
			case LEFT:
				pPlayer->x = p->x+(p->s+pPlayer->s)/2;
				break;
			case RIGHT:
				pPlayer->x = p->x-(p->s+pPlayer->s)/2;
				break;
			}
			pPlayer->p = 1;
			break;
		}
	}
	// 判断子弹和边界的碰撞
	for (Entity *p = pHeadBullets->next; p != NULL; p = p->next)	
	{
		if (freeP != NULL)
		{
			Destroy(pHeadBullets, freeP);
			freeP = NULL;
		}
		if (WallCollide(p, wndWidth, wndHeight))
			freeP = p;
	}
	if (freeP != NULL)
	{
		Destroy(pHeadBullets, freeP);
		freeP = NULL;
	}

	for (Entity *p = pHeadEnemyBullets->next; p != NULL; p = p->next)	
	{
		if (freeP != NULL)
		{
			Destroy(pHeadEnemyBullets, freeP);
			freeP = NULL;
		}
		if (WallCollide(p, wndWidth, wndHeight))
			freeP = p;
	}
	if (freeP != NULL)
	{
		Destroy(pHeadEnemyBullets, freeP);
		freeP = NULL;
	}

	// 玩家和边界的碰撞
	if (WallCollide(pPlayer, wndWidth, wndHeight))
		pPlayer->p = 1;

	if (pPlayer->life <= 0)
	{
		ChangeLevel(FAIL);
	}
	else if (nScore >= GetPassScore())	// 依据得分决定是否过关
	{
		switch (curLevel)
		{
		case LEVEL1:
			ChangeLevel(LEVEL2_OPEN);
			break;
		case LEVEL2:
			ChangeLevel(SUCCEED);
			break;
		}
	}
}