# 
# Sample Makefile for project 2 (COMP30023)
#
# Ryan O'Kane: rokane - 587723



## CC  = Compiler.
## CFLAGS = Compiler flags.
CC = gcc
CFLAGS = -Wall  


## OBJ = Object files.
## SRC = Source files.
## EXE = Executable name.

SRC1 = client.c connect4.c
OBJ1 = client.o connect4.o
EXE1 = client

SRC2 = server.c connect4.c
OBJ2 = server.o connect4.o
EXE2 = server

## Top level target is executable.
$(EXE1):$(OBJ1) 
	$(CC) $(CFLAGS) -o $(EXE1) $(OBJ1) 

## Top level target is executable.
server:$(OBJ2)
	$(CC) $(CFLAGS) -o $(EXE2) $(OBJ2) -lpthread


## Clean: Remove object files and core dump files.
clean:
	/bin/rm $(OBJ1) 
	/bin/rm $(OBJ2) 

## Clobber: Performs Clean and removes executable file.

clobber:	clean
	/bin/rm $(EXE1) 
	/bin/rm $(EXE2)

## Dependencies

server.o:	server.h

