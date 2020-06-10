// Storage.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <stdio.h> 
char GetGlobal();
char Next();
char Last();
char Get();
char c = 'G';				// 定义全局变量c
int main()
{
	char c = 'L';			// 定义局部变量c

	printf("Local c in main function is: %c\n", c);	// 在当前函数中使用的变量c是局部变量

	printf("Global c is: %c\n", GetGlobal());		// 获取全局变量

	printf("Static c in file2 is: %c\n", Get());	// 获取文件2中的静态变量c
	// 函数中的静态变量会保留
	printf("Previous values of static c in file2 are: %c, %c, and %c\n", Last(), Last(), Last());
	printf("Next values of static c in file2 are: %c, %c, and %c\n", Next(), Next(), Next());

	getchar();	// 程序暂停，等待用户输入，以便让用户看到前面的输出结果
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
