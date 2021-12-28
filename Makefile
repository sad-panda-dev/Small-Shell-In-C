#                                                                                                                                                                            
# makefile for smsh                                                                                                                                                          
#                                                                                                                                                                            
CC=gcc
CFLAGS=-g
OBJECTS=smsh1.o splitline.o execute.o builtins.o

smsh1: $(OBJECTS)
        $(CC) $(CFLAGS) -o smsh1 $(OBJECTS)

clean:
        rm $(OBJECTS)
