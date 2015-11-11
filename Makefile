OBJECTS=algorithms.o distances.o dotter.o main.o mazes.o pnger.o printer.o utils.o
CFLAGS= -g -Wall -Werror -O3 -std=c11
LDFLAGS= -lcairo
CC=gcc

all: mazes

mazes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm $(OBJECTS)

.PHONY: clean all
