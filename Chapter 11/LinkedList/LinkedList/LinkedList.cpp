// test.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <crtdbg.h>		// 判断内存泄漏
// 玩家链表节点结构体类型
typedef struct P{
	float hp;
	float mp;
	char name[16];
	struct P *next;
}Player;
// 玩家链表头节点结构体类型
typedef struct{
	int num;
	Player* next;
}PlayerLinklist;
// 向链表中添加一个节点，此处直接插到链表头部
void AddNewPlayer(PlayerLinklist *head, Player *newPlayer)
{
	newPlayer->next = head->next;
	head->next = newPlayer;
	head->num++;
}
// 从链表中删除一个节点，需要注意链表不能断开
void RemovePlayer(PlayerLinklist *head, Player *player)
{
	Player *p = head->next;
	if (p == player)	// 第一个节点是待删除节点
	{
		head->next = p->next;
		free(p);
		head->num--;
		return;
	}
	while (p->next != player && p->next != NULL)
		p = p->next;
	if (p->next == player)	// 删除节点，防止断链
	{
		Player *tmp = p->next->next;
		free(p->next);
		p->next = tmp;
		head->num--;
	}
}
// 在链表中查找指定属性的节点
Player *FindeByName(PlayerLinklist *head, char* name)
{
	Player *p = head->next;
	while (p != NULL && strcmp(p->name, name) != 0)
		p = p->next;
	return p;
}
// 利用递归函数实现，依据玩家的hp对链表中的节点升序排序
// 返回值为排序之后的头节点指针
Player* SortByHP(Player *first)
{
	// 链表中没有节点或者只有一个，则直接返回
	if (first == NULL || first->next == NULL)
		return first;
	Player *minPre = first;	// 保存链表中最小元素的前面一个元素指针
	Player *p = first->next;

	// 将minPre设置为最小的元素前面一个元素的指针
	while(p->next != NULL)
	{
		if (p->next->hp < minPre->next->hp)
			minPre = p;
		p = p->next;
	}
	if (minPre->next->hp < first->hp)	// 比较最小节点和首节点大小
		// 将最小节点插入到首节点前面
	{
		p = minPre->next;
		minPre->next = p->next;
		p->next = first;
		first = p;
	}
	first->next = SortByHP(first->next);// 递归，将后续的节点中最小的节点设为首节点，并重新挂接到链表中
	return first;
}
// 遍历所有节点，打印链表中的玩家信息
void Print(PlayerLinklist *head)
{
	Player* p = head->next;
	while (p != NULL)
	{
		printf("%s,%.2f,%.2f\t",p->name, p->hp, p->mp);
		p = p->next;
	}
	printf("\n");
}
// 释放所有临时占用内存
void Destroy(PlayerLinklist *head)
{
	Player* p = head->next;
	Player* q = p->next;
	while (q != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
	free(head);
}

int main()
{
	srand(time(NULL));		// 随机种子
	// 动态生成玩家链表头
	PlayerLinklist *head = (PlayerLinklist*)malloc(sizeof(PlayerLinklist));
	head->next = NULL;
	head->num = 0;
	int nCand = rand()%20;	// 随机待查找和删除的玩家序号
	char *find = NULL;		// 待查找玩家姓名
	for (int i = 0; i < 20; i++)	// 随机生成20个玩家，并添加到链表中
	{
		Player *p = (Player*)malloc(sizeof(Player));
		p->hp = (rand()%101)/100.0f;	// 随机血量
		p->mp = (rand()%101)/100.0f;	// 随机魔法值
		p->name[0] = 'A' + rand()%26;	// 随机的名字，为了简便只使用3个字母
		p->name[1] = 'A' + rand()%26;
		p->name[2] = 'A' + rand()%26;
		p->name[3] = '\0';
		AddNewPlayer(head, p);			// 将新生成的玩家加入列表
		if (nCand == i)
			find = p->name;
	}
	Print(head);						// 打印生成的玩家链表

	Player *p = FindeByName(head, find);// 搜索这个玩家
	printf("The player found:name:%s,hp:%.2f,mp:%.2f\n", p->name, p->hp, p->mp);

	RemovePlayer(head, p);				// 删除这个玩家
	printf("Remove it from the list\n");
	Print(head);

	printf("Sort the list by hp\n");	// 排序
	head->next = SortByHP(head->next);
	Print(head);
	// 该语句可以在程序调试运行后的output窗口中输出内存泄漏信息
	// 读者尝试将后面的Destroy函数调用语句删除，就会导致内存泄漏
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	Destroy(head);						// 善后，将所有临时创建内存删除

	getchar();
	return 0;
}
