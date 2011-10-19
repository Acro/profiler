/*
 
============================================================================
 Name        : evaluate.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 
============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp_trace;
struct data_stack{
		char function[200];
		//float begin;
		//float end;
		int beginSec;
		int beginUSec;
		int endSec;
		int endUSec;
		struct data_stack* next;

};
struct data_stack* put(struct data_stack *stack, struct data_stack *new){
    if (new == NULL) return stack;
	if(stack==NULL){
		stack=new;
		stack->next = NULL;
	}
	else{
		struct data_stack *pos=stack;
		while(pos->next!=NULL){
			pos=pos->next;
		}
		pos->next=new;
		new->next=NULL;
	}
	return stack;
}
struct data_stack* getAndRem(struct data_stack *stack){
	if(stack!=NULL){
		struct data_stack *ret;
		if(stack->next!=NULL){
			struct data_stack *pos=stack;
			while(pos->next->next != NULL){
				pos=pos->next;
			}
			ret=pos->next;
			pos->next=NULL;
			return ret;
		}else {
			ret=stack;
			stack=NULL;
			return ret;
		}
	}
	return NULL;
}

struct data_stack* get(struct data_stack *stack){
	if(stack==NULL)return NULL;
	struct data_stack *pos=stack;
	while(pos->next!=NULL){
		pos=pos->next;
	}
	return pos;
}

void printStack(struct data_stack *stack){
	struct data_stack *toDel;
	while(stack!=NULL){
		printf("Function %s %ds %dms\n", stack->function,(stack->endSec-stack->beginSec),(stack->endUSec-stack->beginUSec));
		toDel=stack;
		stack=stack->next;
		free(toDel);
	}
}

int main(void) {

	fp_trace = fopen("trace.out", "r");

	char line[251]={0};

	struct data_stack *inProgressStack=NULL;
	struct data_stack *finalStack=NULL;
	char empty[200];
	while(NULL!=fgets(line,250,fp_trace)){
		char function[200]={0};
		int sec;
		int usec;
		char type;
		sscanf(line,"%c %*s %s %d %d", &type, function, &sec, &usec);
		struct data_stack *data;
		if(type=='e'){
			data=malloc(sizeof(struct data_stack));
			strcpy(data->function,function);
			data->beginSec=sec;
			data->beginUSec=usec;
			inProgressStack=put(inProgressStack,data);
		}
		else if(type=='x')
		{
			//if(!strcmp(function,data->function)) return 2;
			data=getAndRem(inProgressStack);
			data->endSec=sec;
			data->endUSec=usec;
			finalStack=put(finalStack,data);
		}else return 1;
		//sscanf(line,"");
	}
	printStack(finalStack);
	//getc();
	fclose(fp_trace);
	return EXIT_SUCCESS;
}

