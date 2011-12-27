#!/bin/sh
echo "$1" > func.txt
PARAMS=${#1}

gcc -finstrument-functions -g -c -o demo.o demo.c
#gcc -c -o trace.o trace.c
#gcc -c -o trace.o trace_20111223.c

if test $PARAMS -eq 0
then
	gcc -c -o trace.o trace_full_20111223.c
	#gcc -c -o trace.o trace.c
else
	gcc -c -o trace.o trace_20111226.c
fi

gcc demo.o trace.o -o main
echo Run with ./main
