OBJECTS=algorithms.o distances.o dotter.o main.o mazes.o pnger.o printer.o utils.o
CFLAGS= -g -Wall -Werror -O3 -std=c11
LDFLAGS= -lcairo
GITSHA=$(git rev-parse HEAD)
GITDIRTY=$(git status --porcelain 2> /dev/null)
CC=gcc

all: mazes

mazes: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

container: mazes
	docker build -t mazes-c .

push-container: container
ifeq ($(GITDIRTY),"")
	docker tag mazes-c quay.io/joeatwork_/mazes-c:$GITSHA
	docker push quay.io/joeatwork/mazes-c
else
	@echo "I won't push the container from a dirty repo"
endif

clean:
	rm $(OBJECTS)

.PHONY: clean all
