#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _MyStruct
{
	int data1;
	char data2;
	int data3;
	char data4;
	char ch[0];
}MyStruct;

int main()
{
	int nSize = sizeof(MyStruct) + sizeof(int);
	char *t1 = calloc(nSize, 1);
	MyStruct *t = (MyStruct *) t1;
	memset(t1, 0, nSize);
	t->data1 = 1;
	t->data2 = 1;
	t->data3 = 1;
	t->data4 = 1;
	*((int *) t->ch) = 256;
	
	printf("(%d, %d, %d, %d, %d)\n", t->data1, t->data2, t->data3, t->data4, *((int*) t->ch));
	printf("(%x, %x, %x, %x)\n", t->ch[0], t->ch[1], t->ch[2], t->ch[3]);
	
	return 0;
}
