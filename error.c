#include <stdio.h>

int main (){
	int a, b;
	int array[10];
	for(int i = 0; i < 100; i++) array[i] = 0;
	scanf("%d %d", &a, &b);
	printf("%d\n", a+b);
	return 0;
}
