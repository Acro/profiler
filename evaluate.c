/*
 
============================================================================
 Name        : evaluate.c
 Author      :
 Version     :
 Copyright   :
 Description : Output analyzer
 
============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp_trace;

struct data_stack {
		char name[200];
		double beginSec;
		double totalSec;
		int recursion;
		struct data_stack* next;
};

struct data_stack* functionEntered(struct data_stack *stack, struct data_stack *new) {
	if (new == NULL) {
		return stack;
	}

	if (stack == NULL) { 	// if we just begin, we just initialize the stack using the first record
		stack = new;
		stack->next = NULL;
		stack->recursion = 1;
		stack->totalSec = 0;
	} else {		// otherwise we are going to find out, if this is recursion function, or just another function call within some other function
		struct data_stack *pos = stack;

		while(pos != NULL) { // testing whether function is already present on the stack
			if (strcmp(pos->name, new->name) == 0) { // this means that this function is called recursively (or there are 2+ consecutive function calls)
				if (pos->recursion > 0) {
					pos->recursion += 1;
				} else { // no recursion, different function call
					pos->beginSec = new->beginSec;
					pos->recursion = 1;
				}
				free(new); // deleting temporary data_stack
				return stack;
			}
			pos = pos->next;
		}

		pos = stack; // returning back to the first record on the stack

		while(pos->next != NULL) { // iterating to find the last record
			pos = pos->next;
		}

		pos->next = new;
		new->next = NULL;
		new->totalSec = 0;
		new->recursion = 1;
	}
	return stack;
}

void functionExited(struct data_stack *stack, struct data_stack *new)
{
	struct data_stack *pos = stack;

	while(pos != NULL) {
		if (strcmp(pos->name, new->name) == 0) { // if we just exited the function and previous record has the same name
			if (pos->recursion > 1) {
				pos->recursion -= 1;
			} else {
				pos->totalSec += new->beginSec - pos->beginSec;
				pos->recursion = 0;
			}
			free(new);
			return;
		}
		pos = pos->next;
	}

	free(new);
}


void printStack(struct data_stack *stack){
	FILE *fp;
	char functionName[250];
	struct data_stack *toDel;

	while(stack != NULL) {
		char cmd[250];
		strcpy(cmd, "addr2line -f -e ");
		strcat(cmd, "main");
		strcat(cmd, " ");
		strcat(cmd, stack->name);
		strcat(cmd," | head -1");

		fp = popen( cmd , "r");
		fgets(functionName, sizeof(functionName)-1, fp);
		pclose(fp);

		printf("Function %s %fs\n", functionName, stack->totalSec);
		toDel = stack;
		stack = stack->next;
		free(toDel); // deleting the records on stack while printing them
	}

}

struct data_stack* load(char* file_name) {
	fp_trace = fopen(file_name, "r");
	if(fp_trace == NULL) {
		return;
	}

	char line[256] = {0};
	struct data_stack *out = NULL;

	while(NULL != fgets(line, 255, fp_trace)) {
		char name[200] = {0};
		int sec;
		int usec;
		char type;

		sscanf(line, "%c %s %*s %d %d", &type, name, &sec, &usec);
		struct data_stack *data;

		data = malloc(sizeof(struct data_stack));
		strcpy(data->name, name);
		data->beginSec = sec + (double)(usec)/1000000;

		if(type == 'e') {
			out = functionEntered(out, data);
		} else if(type == 'x') {
			functionExited(out, data);
		} else {
			return;
		}
	}

	fclose(fp_trace);

	return out;
}

int main(int argc, char **argv) {
	struct data_stack* functions = NULL;

	if(argc > 1) {	
		functions = load(argv[1]);
	} else {
		functions = load("trace.out");
	}

	printStack(functions);

	return EXIT_SUCCESS;
}

