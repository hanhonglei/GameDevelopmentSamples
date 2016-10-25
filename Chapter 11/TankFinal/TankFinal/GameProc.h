#ifndef _GAME_PROC_H_ // 这是为了防止头文件被重复包含。
#define _GAME_PROC_H_

#include "Entity.h"

// 游戏的关卡
enum LEVEL {OPEN, LEVEL1, LEVEL2_OPEN, LEVEL2, SUCCEED, FAIL};
// 处理按键
void EnterKey(int key);
// 新建各个链表的头结点
void NewEntity();
// 改变窗口大小的通知函数
void ChangeWndSize(int w, int h);
// 转换关卡
void ChangeLevel(LEVEL newL);
// 重置玩家信息
void ResetPlayer(Entity *player);
// 完成对游戏的绘制
void Draw(HDC hdc, HWND hWnd);
// 绘制整个游戏场景,在其中调用各自的绘制函数完成绘制
void DrawScene(HDC hdc, LEVEL curL);
// 完成游戏内容的绘制
void DrawGameScene(HDC hdc);
// 更新各种游戏信息,定时器会触发这个函数
void Update(int ts);
// 生成一个敌人
void EnemyBirth();
// 将动态分配的内容进行释放
void Destroy();
// 更新游戏场景中的动画元素
void NextFrame(int ts);

#endif
