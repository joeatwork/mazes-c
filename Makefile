OBJECTS=main.o mazes.o utils.o printer.o
CFLAGS= -g -Wall -Werror -O3 -std=c11
CC=gcc-4.7

all: mazes

mazes: $(OBJECTS)

clean:
	rm $(OBJECTS)