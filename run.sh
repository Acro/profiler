#!/bin/sh

gcc -finstrument-functions -g -c -o demo.o demo.c
gcc -c -o trace.o trace.c
gcc demo.o trace.o -o main
echo Run with ./main
