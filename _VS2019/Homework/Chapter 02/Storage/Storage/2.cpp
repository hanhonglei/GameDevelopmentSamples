static char c = '5';	// 只在本文件中有效的变量c
char Next()		
{ 
	static char c2 = c;	// 静态局部变量，函数调用完毕以后，变量不会被删除
	c2 = c2 + 1;
	return c2;
} 
char Last() 
{ 
	static char c2 = c; // 静态局部变量，函数调用完毕以后，变量不会被删除
	c2 = c2 - 1;
	return c2;
} 
char Get()				// 返回只在当前文件有效的变量c
{ 
	return c;
} 
