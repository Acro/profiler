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
struct data_stack{

		char name[200];
		double beginSec;
		double totalSec;
		double callingTime;
		int recursion;
		struct data_stack* next;
};

struct data_stack* functionEntered(struct data_stack *stack, struct 
data_stack *new)
{

    if (new == NULL) return stack;

	if (stack == NULL)
	{
		stack = new;
		stack->next = NULL;
		stack->recursion = 1;
		stack->totalSec = 0;
		stack->callingTime = 0;
	}
	else
	{
		struct data_stack *pos = stack;

        //If function is present on the stack, it will modify it
		while(pos != NULL)
		{
		    if (strcmp(pos->name, new->name) == 0)
		    {
                    if (pos->recursion > 0) pos->recursion += 1;
                    else
                    {
                        pos->beginSec = new->beginSec;
                        pos->recursion = 1;
                    }
                    free(new);
                    return stack;
		    }
			pos = pos->next;
		}

        //Else add function to the end
		pos = stack;

		while(pos->next != NULL)
		{
			pos = pos->next;
		}

		pos->next = new;
		new->next = NULL;
		new->totalSec = 0;
		new->recursion = 1;
		new->callingTime = 0;
	}
	return stack;
}

void functionExited(struct data_stack *stack, struct data_stack *new)
{
    struct data_stack *pos = stack;
	double time = 0;
	while(pos != NULL)
	{
	    if (strcmp(pos->name, new->name) == 0)
	    {
	        if (pos->recursion > 1) pos->recursion -= 1;
		else
		{
		time = new->beginSec - pos->beginSec;
		pos->totalSec += time;
                pos->recursion = 0;
            }
	        }
        pos = pos->next;
    }
	
	pos = stack;
	while (pos != NULL)
	{
		if (pos->recursion > 0 && strcmp(pos->name,new->name)!=0)
		pos->callingTime += time;
		pos = pos->next;
	}
	

    free(new);
}


void printStack(struct data_stack *stack){
    FILE *fp;
    char functionName[250];

	struct data_stack *toDel;
	while(stack!=NULL)
	{
	    char cmd[250];
	    strcpy(cmd, "addr2line -f -e ");
	    strcat(cmd, "main");
	    strcat(cmd, " ");
	    strcat(cmd, stack->name);
	    strcat(cmd," | head -1");
	    fp = popen( cmd , "r");

	    fgets(functionName, sizeof(functionName)-1, fp);

		printf("Function %s %fs incl., %fs clear\n", functionName, 
stack->totalSec, stack->totalSec - stack->callingTime);
		toDel=stack;
		stack=stack->next;
		free(toDel);
	}

    pclose(fp);
}

int main(void) {

	fp_trace = fopen("trace.out", "r");

	char line[251]={0};

	struct data_stack *functions=NULL;

	while(NULL!=fgets(line,250,fp_trace)){

		char name[200]={0};
		int sec;
		int usec;
		char type;

		sscanf(line,"%c %s %*s %d %d", &type, name, &sec, &usec);
		struct data_stack *data;

		data=malloc(sizeof(struct data_stack));
		strcpy(data->name, name);
        data->beginSec=sec + (double)(usec)/1000000;

		if(type=='e')
		{
			functions=functionEntered(functions,data);
		}
		else if(type=='x')
		{
			functionExited(functions,data);
		}
		else return 1;
	}
	printStack(functions);
	fclose(fp_trace);

	return EXIT_SUCCESS;
}

