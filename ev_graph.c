#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp_trace;

struct data_stack {
		int sec;
		int usec;
};

void load(char* file_name) {
	fp_trace = fopen(file_name, "r");

	int recursion = 0;
	struct data_stack data;

	if(fp_trace == NULL) {
		return;
	}

	char line[256] = {0};

	while(NULL != fgets(line, 255, fp_trace)) {
		char name[200] = {0};
		int sec;
		int usec;
		char type;

		sscanf(line, "%c %s %*s %d %d", &type, name, &sec, &usec);

		if(recursion == 0 && type == 'e') {
			data.sec = sec;	
			data.usec = usec;
			recursion++;
		} else if(recursion > 1 && type == 'x') {
			recursion--;
		}

		if(recursion == 1 && type == 'x') {
			if(data.sec == sec) {
				printf("0.%06d\n", usec - data.usec);
			} else if(sec > data.sec) {
				int u = (1000000 - data.sec) + usec;
				int s = (sec - 1) - data.sec;
				if(u > 999999) {
					s += (u - (u % 1000000)) / 1000000;
					u = u % 1000000;
				}
				printf("%d.%06d\n", s, u);
			}
			recursion--;
		}
	}

	fclose(fp_trace);
}

int main(int argc, char **argv) {
	if(argc > 1) {	
		load(argv[1]);
	} else {
		load("trace.out");
	}

	return EXIT_SUCCESS;
}

