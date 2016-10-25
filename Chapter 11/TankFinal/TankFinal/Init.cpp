/************************************************************************/
/*	该文件主要实现游戏初始化及游戏中的物体初始化功能
	也开发出来一些函数，用于获取游戏中的某些参数 */
/************************************************************************/

#include "stdafx.h"
#include "Init.h"
#include <stdio.h>
#include "resource.h"

WCHAR *fileIni = L"Resources\\Init.ini";	// 用于保存当前所要载入的Ini文件名
HBITMAP tank1[4],tank2[4],tank3[4],bulletPic[4],block=NULL,background=NULL;	// 各种物体的图片
HBITMAP logo=NULL, succ_pic=NULL, fail_pic=NULL;	// logo图片和成功失败图片

int timeStep = 20;			// 定时器时间间隔
int animStep = 100;			// 动画帧更新定时器时间间隔
int nLife = 3;				// 玩家生命
int enemyFirePer = 300;		// 敌人发射子弹的随机比例
int enemyDirPer = 200;		// 敌人随机改变方向的比例
int birthTime = 5000;		// 敌人的出生时间间隔
int nPassScore = 10;		// 过关分数
extern HINSTANCE hInst;		// current instance,在主文件中的全局变量

// 返回动画帧更新时间间隔
int GetAnimStep()
{
	return animStep;
}
// 返回logo图片句柄
HBITMAP GetLogoPic()
{
	return logo;
}
// 返回胜利图片
HBITMAP GetSuccPic()
{
	return succ_pic;
}
// 返回失败图片
HBITMAP GetFailPic()
{
	return fail_pic;
}
// 返回当前的帧时间间隔
int GetTimeStep()
{
	return timeStep;
}
// 返回敌人出生几率
int GetBirthTime()
{
	return birthTime;
}
// 返回当前的背景图片句柄
HBITMAP GetBackPic()
{
	return background;
}
// 是否随机开火
int RandomFire()
{
	return ((rand()%enemyFirePer) == 0);
}
// 是否随机改变方向
int RandomDir()
{
	return (rand()%enemyDirPer == 0);
}
// 获取通关分数
int GetPassScore()
{
	return nPassScore;
}
// 按照指定参数生成敌人,并添加到敌人链表中
void EnemyBirth(Entity* headEnemy, int type, int w)
{
	Entity *ent = (Entity*)malloc(sizeof(Entity));
	memcpy(ent, headEnemy, sizeof(Entity));
	ent->next = headEnemy->next;
	headEnemy->next = ent;
	ent->dir = Dir(UP+rand()%4);
	ent->x = (rand()%3)*(w-ent->s)/2 + ent->s/2;
	ent->y = ent->s;

	switch (type)
	{
	case 0:
		ent->v = GetPrivateProfileInt(L"Enemy", L"vels",2, fileIni);
		memcpy(ent->a, tank2, 4*sizeof(HBITMAP));
		ent->life = 1;
		break;
	case 1:
		ent->v = GetPrivateProfileInt(L"Enemy", L"velf",4, fileIni);
		memcpy(ent->a, tank3, 4*sizeof(HBITMAP));
		ent->life = 2;
		break;
	}
}
// 依据所处的关卡,通过ini文件读取部分初始化信息
void ReadIni(LEVEL l)
{
	switch (l)
	{
	case OPEN:
		fileIni = L"Resources\\Init.ini";
		break;
	case LEVEL1:
		fileIni = L"Resources\\Init.ini";
		break;
	case LEVEL2_OPEN:
		break;
	case LEVEL2:
		fileIni = L"Resources\\Init2.ini";
		break;
	default:
		fileIni = L"Resources\\Init.ini";
		return;
	}
	animStep=GetPrivateProfileInt(L"Global", L"animStep",animStep, fileIni);
	timeStep=GetPrivateProfileInt(L"Global", L"timeStep",timeStep, fileIni);
	nPassScore=GetPrivateProfileInt(L"Global", L"nPassScore",nPassScore, fileIni);
	enemyFirePer = GetPrivateProfileInt(L"Enemy", L"firePer",enemyFirePer, fileIni);
	enemyDirPer = GetPrivateProfileInt(L"Enemy", L"dirPer",enemyDirPer, fileIni);
	birthTime = GetPrivateProfileInt(L"Enemy", L"birthTime",birthTime, fileIni);
	nLife = GetPrivateProfileInt(L"Player", L"nLife",nLife, fileIni);
}
// 依据所处的关卡，读取不同的地图配置文件,标记为1的地方有障碍物
void InitMap(LEVEL l, Entity *headBlock)
{
	char *fileName = "Resources\\Map.txt";
	switch (l)
	{
	case LEVEL1:
		fileName = "Resources\\Map.txt";
		break;
	case LEVEL2:
		fileName = "Resources\\Map2.txt";
		break;
	default:
		return;
	}
	FILE *f = fopen(fileName, "r");
	if (f == NULL)
		return;
	char line[64];
	int nLine = 0;
	Entity *newBlock = NULL;
	while(!feof(f))
	{
		fgets(line, 64, f);
		for (int i = 0; line[i] != '\0'; i++)
		{
			if (line[i] == '1')
			{
				newBlock = (Entity*)malloc(sizeof(Entity));
				memcpy(newBlock, headBlock, sizeof(Entity));
				newBlock->x = newBlock->s/2+newBlock->s*i;
				newBlock->y = newBlock->s/2+newBlock->s*nLine;
				newBlock->next = headBlock->next;
				headBlock->next = newBlock;
			}
		}
		nLine++;
	}
	fclose(f);
}
// 依据不同的关卡，对游戏参数进行初始化
void Init(LEVEL l)
{
	switch (l)
	{
	case OPEN:					
		logo = (logo==NULL)?(HBITMAP)LoadImage( NULL, L"Resources\\logo.bmp", IMAGE_BITMAP, 0, 0,
			LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE ):logo;
		break;
	case LEVEL2_OPEN:	
	case SUCCEED:
		succ_pic = (succ_pic==NULL)?(HBITMAP)LoadImage( NULL, L"Resources\\success.bmp", IMAGE_BITMAP, 0, 0,
			LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE ):succ_pic;
		break;
	case FAIL:
		fail_pic = (fail_pic==NULL)?(HBITMAP)LoadImage( NULL, L"Resources\\fail.bmp", IMAGE_BITMAP, 0, 0,
			LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE ):fail_pic;
		break;
	}
	// 首先读取配置文件中的参数填充全局变量
	if (l != LEVEL1 && l != LEVEL2)
		return;

	ReadIni(l);

	// 从硬盘载入图片
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
	if (LEVEL1 == l)
	{
		block=LoadBitmap (hInst,MAKEINTRESOURCE(IDB_GRASS));		// 从资源文件中下载图片
		background = (HBITMAP)LoadImage( NULL, L"Resources\\back.bmp", IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );	// 从硬盘中读取背景文件
	}
	else if (LEVEL2 == l)
	{
		block=LoadBitmap (hInst,MAKEINTRESOURCE(IDB_ROCK)); 		//从资源文件中下载图片
		background = (HBITMAP)LoadImage( NULL, L"Resources\\back2.bmp", IMAGE_BITMAP, 0, 0,
			LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );// 从硬盘中读取背景文件
	}
}
// 关卡中的各种实体链表头参数进行初始化
void InitEntity(Entity *headEnemy, Entity *headBlock, Entity *headBullet, Entity *headEnemyBullet, Entity *player, LEVEL l)
{
	// 删除上一关中的链表元素
	Destroy(headEnemy);
	Destroy(headBlock);
	Destroy(headBullet);
	Destroy(headEnemyBullet);

	// enemy
	headEnemy->s = GetPrivateProfileInt(L"Enemy", L"size",50,fileIni);
	headEnemy->dir = Dir(UP+rand()%4);
	headEnemy->v = rand()%2==0?GetPrivateProfileInt(L"Enemy", L"velf",4, fileIni):GetPrivateProfileInt(L"Enemy", L"vels",2, fileIni);
	memcpy(headEnemy->a, tank2, 4*sizeof(HBITMAP));
	headEnemy->x = 0;
	headEnemy->y = 0;
	headEnemy->p = 0;
	headEnemy->life = 1;
	headEnemy->f = 0;
	headEnemy->frames = GetPrivateProfileInt(L"Enemy", L"frames",1, fileIni);

	// block
	headBlock->s = GetPrivateProfileInt(L"Block", L"size",50,fileIni);
	headBlock->dir = NONE;
	headBlock->v = 0;
	headBlock->a[0] = headBlock->a[1] = headBlock->a[2] = headBlock->a[3] = block;
	headBlock->x = 0;
	headBlock->y = 0;
	headBlock->p = 0;
	headBlock->life = 1;
	headBlock->f = 0;
	headBlock->frames = 1;

	InitMap(l, headBlock);	// 初始化地图

	// bullet
	headBullet->s = GetPrivateProfileInt(L"Bullet", L"size",20, fileIni);
	memcpy(headBullet->a, bulletPic, 4*sizeof(HBITMAP));
	headBullet->dir = NONE;
	headBullet->v = GetPrivateProfileInt(L"Bullet", L"vel",6, fileIni);
	headBullet->x = 0;
	headBullet->p = 0;
	headBullet->y = 0;
	headBullet->life = 1;
	headBullet->f = 0;
	headBullet->frames = GetPrivateProfileInt(L"Bullet", L"frames",1, fileIni);
	headBullet->animD = 1;

	memcpy(headEnemyBullet, headBullet, sizeof(Entity));

	// player
	player->s = GetPrivateProfileInt(L"Player", L"size",50, fileIni);
	memcpy(player->a, tank1, 4*sizeof(HBITMAP));
	player->dir = UP;
	player->v = GetPrivateProfileInt(L"Player", L"vel",2, fileIni);
	player->x = 0;
	player->y = 0;
	player->p = 1;
	player->life = nLife;
	player->f = 0;
	player->frames = GetPrivateProfileInt(L"Player", L"frames",1, fileIni);
}