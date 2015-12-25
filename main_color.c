#include <graphviz/cgraph.h>
#include <stdbool.h>
#include "utils.h"

struct distance_rec {
  Agrec_t header;
  size_t distance;
};

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s [--seed=number]\n", command_name);
  fprintf(stderr, "Annotates the nodes on a graph with a distance from some random spot\n");
  fprintf(stderr, "reads stdin in dot format, writes to stdout\n");
}

int main(int argc, char** argv) {
  char *command_name = argv[0];
  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }

  size_t nodecount = agnnodes(maze); // TODO size+type
  if (nodecount > 0) {

	Agnode_t *found = NULL;
	{
	  size_t at = 0;
	  size_t pick;
	  RANDOM_CHOICE(pick, nodecount);
	  for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze, n)) {
		if (pick == at) {
		  found = n;
		  break;
		}
		at++;
	  }
	}

	if (NULL == found) {
	  ERROR_EXIT("failed to pick a starting node");
	}

	Agnode_t **stack = checked_calloc(nodecount, sizeof(Agnode_t *));
	stack[0] = found;
	size_t stack_size = 1;
	struct distance_rec *dist0 =
	  agbindrec(stack[0], "distance", sizeof(struct distance_rec), false);
	dist0->distance = 0;
	while (stack_size > 0) {
	  Agnode_t *next = stack[stack_size - 1];
	  stack_size--;
	  
	  struct distance_rec *dist = (struct distance_rec *)aggetrec(next, "distance", false);
	  size_t next_distance = dist->distance + 1;
	  for (Agedge_t *e = agfstedge(maze, next); NULL != e; e = agnxtedge(maze, e, next)) {
		Agnode_t *other = e->node;

		if (NULL == aggetrec(other, "distance", 0)) {
		  struct distance_rec *nxt =
			agbindrec(other, "distance", sizeof(struct distance_rec), false);
		  nxt->distance = next_distance;
		  stack[stack_size] = other;
		  stack_size++;
		}
	  }
	}
	free(stack);

	for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze, n)) {
	  struct distance_rec *dist;
	  if (NULL != (dist = (struct distance_rec *)aggetrec(n, "distance", 0))) {
		char attrvalue[12];
		checked_snprintf(attrvalue, sizeof(attrvalue), "%ld", dist->distance);
		agsafeset(n, "color", attrvalue, "");
	  }
	}
  }

  agwrite(maze, stdout);
  agclose(maze);
}
