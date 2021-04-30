AN=$(shell head -1 00_ID.txt)
CLASS=$(shell tail -1 00_ID.txt)

CFLAGS = -Wall -Werror -g 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')
 
# Object files to build 

blather: bl_server.o bl_client.o util.o blather.h
	$(CC) -o blather bl_server.o bl_client.o util.o 
	@echo blather is ready

util.o: util.c blather.h
	$(CC) -c util.c

bl_server.o: bl_server.c blather.h
	$(CC) -c bl_server.c

bl_client.o: bl_client.c blather.h
	$(CC) -c bl_client.c 

clean:
	rm -f blather *.o

#include test_Makefile
