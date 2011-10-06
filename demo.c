#include <stdio.h>
#include <unistd.h>
int main() {
	int i;
	for(i = 0; i < 10; i++) {
		printf("a\n");
		sleep(0.1);
	}
}
