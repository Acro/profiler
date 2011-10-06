#include <stdio.h>
#include <time.h>
#include <sys/time.h>

static FILE *fp_trace;

void 
__attribute__ ((constructor))
trace_begin (void) {
	fp_trace = fopen("trace.out", "w");
}

void
__attribute__ ((destructor))
trace_end (void) {
	if(fp_trace != NULL) {
		fclose(fp_trace);
	}
}

void
__cyg_profile_func_enter (void *func, void *caller) {
	struct timeval time;
	gettimeofday( &time, NULL);
	if(fp_trace != NULL) {
		//fprintf(fp_trace, "e %p %p %lu\n", func, caller, time(NULL) );
		fprintf(fp_trace, "e %p %p %lu %lu\n", func, caller, time.tv_sec, time.tv_usec );
	}
}

void
__cyg_profile_func_exit (void *func, void *caller) {
	struct timeval time;
	gettimeofday( &time, NULL);
	if(fp_trace != NULL) {
		fprintf(fp_trace, "x %p %p %lu %lu\n", func, caller, time.tv_sec, time.tv_usec );
	}
}
