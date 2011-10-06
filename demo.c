#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main() {
	int i = 0, j = 1;
	for(; i < 100000000; i++) {
		j += i;
	}
}
