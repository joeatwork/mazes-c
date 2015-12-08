LIB_OBJECTS=algorithms.o distances.o dotter.o mazes.o pnger.o printer.o utils.o
TEST_OBJECTS=tests.o $(LIB_OBJECTS)
OBJECTS=main.o $(LIB_OBJECTS)
CFLAGS=-I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -g -Wall -Werror -O3 -std=c11
LDFLAGS= -lcairo -lm -lglib-2.0
GITSHA=$(shell git rev-parse HEAD)
GITDIRTY=$(shell git status --porcelain 2> /dev/null)
CC=gcc

all: mazes

mazes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

container: mazes
	docker build -t mazes-c .

push-container: container
ifeq ($(strip $(GITDIRTY)),)
	docker tag mazes-c quay.io/joeatwork/mazes-c:$(GITSHA)
	docker push quay.io/joeatwork/mazes-c
else
	@echo "I won't push the container from a dirty repo"
endif

clean:
	rm *.o tests

run-tests: tests
	./tests

tests: $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) -o $@ $(LDFLAGS)		

.PHONY: all clean container push-container run-tests
