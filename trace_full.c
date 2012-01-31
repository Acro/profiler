#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

struct data {
	unsigned long int sec;
	unsigned int usec;
	void* func;
	void* caller;
	unsigned short state;
	struct data* next;
};
static struct data* start = NULL;
static struct data* last = NULL;
static struct data* out;
static struct timeval current_time;

/*
void 
__attribute__ ((constructor))
trace_begin (void) {

}
*/

void
__attribute__ ((destructor))
trace_end (void) {
	FILE* test = fopen("out.txt", "w");
	FILE* temp = fopen("temp.txt", "w");
	if(test != NULL) {
		struct data* stack = start;
		while(stack != NULL) {
			if(!stack->state) {
				fprintf(test, "e %p %p %lu %d\n", stack->func, stack->caller, stack->sec, stack->usec );
				fprintf(temp, "e %p\n", stack->func);
			} else {
				fprintf(test, "x %p %p %lu %d\n", stack->func, stack->caller, stack->sec, stack->usec );
				fprintf(temp, "x %p\n", stack->func);
			}
			struct data* prev = stack;
			stack = stack->next;
			free(prev);
		}
		fclose(test);
	}
}

void
__cyg_profile_func_enter (void *func, void *caller) {
	out = malloc(sizeof(struct data));
	out->func = func;
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

void
__cyg_profile_func_exit (void *func, void *caller) {
	gettimeofday( &current_time, NULL);

	out = malloc(sizeof(struct data));
	out->func = func;
	out->caller = caller;
	out->sec = current_time.tv_sec;
	out->usec = current_time.tv_usec;
	out->state = 1;
	out->next = NULL;

	last->next = out;
	last = out;
}
