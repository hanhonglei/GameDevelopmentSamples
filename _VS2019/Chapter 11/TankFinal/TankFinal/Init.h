#ifndef _INIT_H_ // 这是为了防止头文件被重复包含。
#define _INIT_H_

#include "GameProc.h"
// 返回动画帧更新时间间隔
int GetAnimStep();
// 返回当前的帧时间间隔
int GetTimeStep();
// 通过ini文件读取当前关卡的部分初始化信息
void ReadIni(LEVEL l);
// 读取地图配置文件,标记为1的地方有障碍物
void InitMap(LEVEL l, Entity *headBlock);
// 依据当前场景的不同对游戏内容进行初始化
void Init(LEVEL l);
// 对指定的参数进行初始化
void InitEntity(Entity *headEnemy, Entity *headBlock, Entity *headBullet, Entity *headEnemyBullet, Entity *player, LEVEL l);
// 获取当前关卡的背景图片句柄
HBITMAP GetBackPic();
// 敌人随机开火
int RandomFire();
// 敌人随机改变方向
int RandomDir();
// 获取随机生成几率
int GetBirthTime();
// 依据敌人类型生成一个敌人,并添加到敌人链表中
void EnemyBirth(Entity* headEnemy, int type, int w);
// 获取通关分数
int GetPassScore();
// 返回logo图片句柄
HBITMAP GetLogoPic();
// 返回胜利图片
HBITMAP GetSuccPic();
// 返回失败图片
HBITMAP GetFailPic();
#endif