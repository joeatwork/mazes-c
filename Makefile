OBJECTS=main.o mazes.o utils.o printer.o pnger.o binary_tree.o
CFLAGS= -g -Wall -Werror -O3 -std=c11
LDFLAGS= -lcairo
CC=gcc-4.7

all: mazes

mazes: $(OBJECTS)

clean:
	rm $(OBJECTS)