#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

static char target_function[256];

struct data {
	long unsigned int sec;
	long unsigned int usec;
	void* func;
	void* caller;
	int state;
	struct data* next;
};

static struct data* start = NULL;
static struct data* last = NULL;
static struct data* out;
static void* profiled_func = NULL;
static struct timeval current_time;
static char buffer[256];

void addr2name(char* name) {
	FILE *fp;
	char cmd[250];
	
	strcpy(cmd, "addr2line -f -e main ");
	strcat(cmd, name);
	strcat(cmd," | head -1");

	fp = popen( cmd , "r");
	fgets(name, 200, fp);
	pclose(fp);
}

void 
__attribute__ ((constructor))
trace_begin (void) {
	FILE* f = fopen("func.txt", "r");
	if(f != NULL) {
		fgets(target_function, 255, f);
		target_function[strlen(target_function)-1] = '\0';
		fclose(f);
	}
}

void
__attribute__ ((destructor))
trace_end (void) {
	FILE* f = fopen("out.txt", "w");
	if(f != NULL) {
		struct data* stack = start;
		while(stack != NULL) {
			if(stack->state == 0) {
				fprintf(f, "e %p %p %lu %lu\n", stack->func, stack->caller, stack->sec, stack->usec );
			} else {
				fprintf(f, "x %p %p %lu %lu\n", stack->func, stack->caller, stack->sec, stack->usec );
			}
			struct data* prev = stack;
			stack = stack->next;
			free(prev);
		}
		fclose(f);
	}
}

void
__cyg_profile_func_enter (void *func, void *caller) {
	if(profiled_func == NULL) {
		sprintf(buffer, "%p", func);
		addr2name(buffer);
		buffer[strlen(buffer)-1] = '\0';

		if(strcmp(buffer, target_function) == 0) {
			profiled_func = func;
		} else {
			return;
		}
	}

	if(profiled_func == func) {
		out = malloc(sizeof(struct data));
		out->func = profiled_func;
		out->caller = caller;

		gettimeofday( &current_time, NULL);

		out->sec = current_time.tv_sec;
		out->usec = current_time.tv_usec;
		out->state = 0;
		out->next = NULL;

		if(start == NULL) {
			start = out;
			last = out;
		} else {
			last->next = out;
			last = out;
		}
	}
}

void
__cyg_profile_func_exit (void *func, void *caller) {
	if(profiled_func == func) {
		gettimeofday( &current_time, NULL);

		out = malloc(sizeof(struct data));
		out->func = profiled_func;
		out->caller = caller;
		out->sec = current_time.tv_sec;
		out->usec = current_time.tv_usec;
		out->state = 1;
		out->next = NULL;

		last->next = out;
		last = out;
	}
}
