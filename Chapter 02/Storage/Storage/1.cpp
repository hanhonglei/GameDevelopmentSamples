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