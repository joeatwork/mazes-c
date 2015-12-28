OBJECTS=algorithms.o layout.o utils.o color.o
CFLAGS=-D_GNU_SOURCE -g -Wall -Werror -O3 -std=c11
LDFLAGS= -lm -lcgraph -lcairo
GITSHA=$(shell git rev-parse HEAD)
GITDIRTY=$(shell git status --porcelain 2> /dev/null)
CC=gcc

all: grid maze print png color scad


.PHONY: all

clean:
	rm *.o

.PHONY: clean

# Tools

color: main_color.o $(OBJECTS)
	$(CC) $(CFLAGS) main_color.o $(OBJECTS) -o $@ $(LDFLAGS)

png: main_png.o $(OBJECTS)
	$(CC) $(CFLAGS) main_png.o $(OBJECTS) -o $@ $(LDFLAGS)

scad: main_scad.o $(OBJECTS)
	$(CC) $(CFLAGS) main_scad.o $(OBJECTS) -o $@ $(LDFLAGS)

print: main_print.o $(OBJECTS)
	$(CC) $(CFLAGS) main_print.o $(OBJECTS) -o $@ $(LDFLAGS)

maze: main_maze.o $(OBJECTS)
	$(CC) $(CFLAGS) main_maze.o $(OBJECTS) -o $@ $(LDFLAGS)

grid: main_grid.o $(OBJECTS)
	$(CC) $(CFLAGS) main_grid.o $(OBJECTS) -o $@ $(LDFLAGS)

# Goofball service

container: color grid maze png print scad
	docker build -t mazes-c .

.PHONY: container

push-container: container
ifeq ($(strip $(GITDIRTY)),)
	docker tag mazes-c quay.io/joeatwork/mazes-c:$(GITSHA)
	docker push quay.io/joeatwork/mazes-c
else
	@echo "I won't push the container from a dirty repo"
endif

.PHONY: push-container

# Tests

run-tests: tests
	./tests

.PHONY: run-tests

tests: tests.o $(OBJECTS)
	$(CC) $(CFLAGS) tests.o $(OBJECTS) -o $@ $(LDFLAGS) -lglib-2.0

.PHONY: tests

tests.o: tests.c
	$(CC) -c $(CFLAGS) -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include $(CPPFLAGS) -o $@ $<
