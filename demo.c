#include <stdio.h>
#include <unistd.h>
#include <math.h>

void function(int n)
{
	//if (n==0) return;
	
	printf("test");
	//function(n-1);
}

int main() {
	int i = 0, j = 1;
	for(; i < 3; i++) {
		j += i;
		sleep(1);
	}
	function(1);
	function(2);
	function(3);
}
