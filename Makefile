AN=$(shell head -1 00_ID.txt)
CLASS=$(shell tail -1 00_ID.txt)

CFLAGS = -Wall -Werror -g 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')
 
# Object files to build 

commando: cmd.o cmdcol.o 
	$(CC) -o commando cmd.o cmdcol.o 
	@echo commando is ready
cmd.o: cmd.c commando.h
	$(CC) -c cmd.c 
cmdcol.o: cmdcol.o commando.h
	$(CC) -c cmdcol.c
util.o: util.o commando.h
	$(CC) -c util.c
clean:
	rm -f commando *.o

include test_Makefile