static char c = '5';	// ֻ�ڱ��ļ�����Ч�ı���c
char Next()		
{ 
	static char c2 = c;	// ��̬�ֲ�������������������Ժ󣬱������ᱻɾ��
	c2 = c2 + 1;
	return c2;
} 
char Last() 
{ 
	static char c2 = c; // ��̬�ֲ�������������������Ժ󣬱������ᱻɾ��
	c2 = c2 - 1;
	return c2;
} 
char Get()				// ����ֻ�ڵ�ǰ�ļ���Ч�ı���c
{ 
	return c;
} 
