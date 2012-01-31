#include <stdio.h>
#include <unistd.h>
#include <math.h>

void recursF(int n)
{
	if (n==0) return;
	
	printf("test");
	recursF(n-1);
}
void testFunc()
{
	printf("Yes");
	//sleep(1);
}

int main() {
	int i = 0, j = 1;
	for(; i < 3; i++) {
		j += i;
		sleep(1);
	}

	testFunc(1);	

	for(i = 0; i < 1000; i++) {
		testFunc();
	}

	recursF(2);
	recursF(10000);
}
