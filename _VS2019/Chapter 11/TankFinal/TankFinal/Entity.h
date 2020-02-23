#ifndef _ENTITY_H_ // 这是为了防止头文件被重复包含。
#define _ENTITY_H_

enum Dir{UP, DOWN, LEFT, RIGHT, NONE};	// 运动物体可能的运动方向
typedef struct	entity		// 游戏中的实体结构体
{
	int x, y;				// 位置
	Dir dir;				// 方向
	int v;					// 速率
	int s;					// 边长，正方形
	int p;					// 是否停止,只有玩家才可能停止
	HBITMAP a[4];			// 位图,4个方向
	int life;				// 保存当前生命值，生命值为0时，需要删除
	struct	entity *next;	// 用于组成链表，指向链表中的下一个物体
	unsigned short f;		// 当前帧
	unsigned short frames;	// 总帧数
	int animD;				// 控制当前动画帧的下一帧方向
}Entity;

// 绘制链表中的每个实体
void DrawEntities(HDC hdc, const Entity *head);
// 绘制参数指定的游戏实体
void DrawEntity(HDC hdc, const Entity *ent);
// 对链表中的每个实体进行移动
void MoveEntities(Entity *head, int ts);
// 对特定游戏实体进行移动
void Move(Entity *ent, int ts);
// 特定实体进行射击操作
void Fire(const Entity* ent, Entity *pHeadB);
// 将特定的实体从链表中删除
void Destroy(Entity *pHead, Entity* ent);
// 删除头结点外的整个链表
void Destroy(Entity *pHead);
// 判断两个实体是否发生碰撞,以正方体之间发生碰撞来判断
int IsCollide(const Entity *ent1, const Entity *ent2);
// 判断特定实体是否和边界发生碰撞
int WallCollide(Entity *ent, int w, int h);
// 将链表中的实体当前帧设置为下一帧
void NextFrameEntites(Entity *head, int ts);
// 将特定实体设置为下一帧
void NextFrame(Entity *ent, int ts);

#endif
