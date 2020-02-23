// TankPlus.cpp : Defines the entry point for the application.
//
/************************************************************************/
/*	关于游戏实体具体操作的功能 */
/************************************************************************/
#include "stdafx.h"
#include "Entity.h"
// 绘制链表中的每个实体
void DrawEntities(HDC hdc, const Entity *head)
{
	Entity* ent = head->next;
	while(ent != NULL)
	{
		DrawEntity(hdc, ent);
		ent = ent->next;
	}
}
// 绘制参数指定的游戏实体
void DrawEntity(HDC hdc, const Entity *ent)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP bmp = ent->a[int(ent->dir)%4];
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, bmp);
	BITMAP bm;
	GetObject(bmp, sizeof(bm), &bm);
	SetStretchBltMode(hdc,STRETCH_HALFTONE);
	// 添加动画效果，绘制当前帧
	TransparentBlt(hdc,ent->x-ent->s/2, ent->y-ent->s/2, ent->s, ent->s, hdcMem, ent->f*bm.bmWidth/(ent->frames), 0, bm.bmWidth/ent->frames, bm.bmHeight,RGB(255,255,255)); // 白色作为透明色，Library Msimg32.lib
	SelectObject(hdcMem, hbmOld);
	DeleteDC(hdcMem);
}
// 将链表中的实体当前帧设置为下一帧
void NextFrameEntites(Entity *head, int ts)
{
	if (head == NULL)
		return;
	Entity* ent = head->next;
	while(ent != NULL)
	{
		NextFrame(ent, ts);
		ent = ent->next;
	}
}
// 将特定实体设置为下一帧
void NextFrame(Entity *ent, int ts)
{
	if (ent == NULL)
		return;
	if (ent->f == ent->frames - 1)
		ent->animD = -1;		// 向左选择下一帧
	else if (ent->f == 0)
		ent->animD = 1;			// 向右选择下一帧

	ent->f = (ent->f+ent->animD)%ent->frames;
}
// 对链表中的实体进行移动
void MoveEntities(Entity *head, int ts)
{
	if (head == NULL)
		return;
	Entity* ent = head->next;
	while(ent != NULL)
	{
		Move(ent, ts);
		ent = ent->next;
	}
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
void Fire(const Entity* ent, Entity *pHeadB)
{
	Entity *newBullet = (Entity*)malloc(sizeof(Entity));
	memcpy(newBullet, pHeadB, sizeof(Entity));
	newBullet->dir = ent->dir;
	newBullet->x = ent->x;
	newBullet->y = ent->y;
	switch(ent->dir)
	{
	case UP:
		newBullet->y -= ent->s;
		break;
	case DOWN:
		newBullet->y += ent->s;
		break;
	case LEFT:
		newBullet->x -= ent->s;
		break;
	case RIGHT:
		newBullet->x += ent->s;
		break;
	}
	newBullet->next = pHeadB->next;
	pHeadB->next = newBullet;
}
// 将特定的实体从链表中删除
void Destroy(Entity *pHead, Entity* ent)
{
	Entity *p = pHead;
	Entity *pn = pHead->next;
	while(pn != NULL)
	{
		if (pn == ent)
		{
			p->next = pn->next;
			free(pn);
			return;
		}
		p = pn;
		pn = pn->next;
	}
}
// 删除头结点外的整个链表
void Destroy(Entity *pHead)
{
	if (pHead == NULL)	return;
	Entity *p = pHead;
	Entity *pn = pHead->next;
	while(pn != NULL)
	{
		p->next = pn->next;
		free(pn);
		pn = p->next;
	}
	pHead->next = NULL;
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
int WallCollide(Entity *ent, int w, int h)
{
	int bC = 0;
	switch(ent->dir)
	{
	case UP:
		if ((ent->y - ent->s/2) < 0)	// 上边界
		{
			bC = 1;
			ent->y = ent->s/2;
		}
		break;
	case DOWN:
		if ((ent->y + ent->s/2) > h)	// 下边界
		{
			bC = 1;
			ent->y = h - ent->s/2;
		}
		break;
	case LEFT:
		if((ent->x - ent->s/2) < 0)		// 左边界
		{
			bC = 1;
			ent->x = ent->s/2;
		}
		break;
	case RIGHT:
		if((ent->x + ent->s/2) > w)		// 右边界
		{
			bC = 1;
			ent->x = w - ent->s/2;
		}
		break;
	}
	return bC;
}
