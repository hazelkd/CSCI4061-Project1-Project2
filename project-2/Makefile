AN=$(shell head -1 00_ID.txt)
CLASS=$(shell tail -1 00_ID.txt)

CFLAGS = -Wall -Werror -g 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')
 
# Object files to build 

bl_server: bl_server.c server_funcs.c util.c blather.h
	$(CC) -o bl_server bl_server.c server_funcs.c util.c -lpthread
	@echo bl_server is ready

bl_client: bl_client.c util.c simpio.c blather.h 
	$(CC) -o bl_client bl_client.c util.c simpio.c -lpthread
	@echo bl_client is ready

clean:
	rm -f blather *.o

include test_Makefile
