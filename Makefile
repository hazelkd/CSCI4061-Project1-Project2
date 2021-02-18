AN=$(shell head -1 00_ID.txt)
CLASS=$(shell tail -1 00_ID.txt)

CFLAGS = -Wall -Werror -g 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')
 
# Object files to build 

commando: cmd.o cmdcol.o commando.o commando.h
	$(CC) -o commando commando.o cmd.o cmdcol.o 
	@echo commando is ready
commando.o: commando.c commando.h
	$(CC) -c commando.c

cmd.o: cmd.c commando.h
	$(CC) -c cmd.c 

cmdcol.o: cmdcol.c commando.h
	$(CC) -c cmdcol.c

clean:
	rm -f commando *.o

include test_Makefile