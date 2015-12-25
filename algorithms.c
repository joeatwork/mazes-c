#include <stdbool.h>
#include <stdlib.h>
#include "algorithms.h"
#include "utils.h"

#define NEIGHBORS_MAX 12
void mazes_generate_backtracker(Agraph_t *maze) {
  size_t stack_max = agnnodes(maze);
  if (stack_max == 0) {
	return;
  }
  
  size_t stack_depth = 0;
  Agnode_t **stack = checked_calloc(stack_max, sizeof(Agnode_t *));
  Agedge_t *neighbors[NEIGHBORS_MAX];

  {
	size_t i = 0;
	for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze,n)) {
	  stack[i] = n;
	  i++;
	}

	size_t choice_ix;
	RANDOM_CHOICE(choice_ix, stack_max);
	stack[0] = stack[choice_ix];
	agbindrec(stack[0], "mark", sizeof(Agrec_t), false);
	stack_depth = 1;
  }

  while (0 < stack_depth) {	
	Agnode_t *top = stack[stack_depth - 1];
	size_t neighbors_count = 0;
	for (Agedge_t *e = agfstedge(maze, top);
		 NULL != e && neighbors_count < NEIGHBORS_MAX;
		 e = agnxtedge(maze, e, top)) {
	  Agnode_t *other;
	  other = e->node;
	  
	  if (NULL == aggetrec(other, "mark", 0) && other != top) {
		neighbors[neighbors_count] = e;
		neighbors_count++;
	  }
	}

	if (0 == neighbors_count) {
	  stack_depth = stack_depth - 1;
	} else {
	  size_t neighbor_ix;
	  RANDOM_CHOICE(neighbor_ix, neighbors_count);
	  Agedge_t *neighbor_edge = neighbors[neighbor_ix];
	  stack[stack_depth] = neighbor_edge->node;
	  agbindrec(stack[stack_depth], "mark", sizeof(Agrec_t), false);
	  stack_depth++;
	  agbindrec(neighbor_edge, "keep", sizeof(Agrec_t), false);
	}
  }

  for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze,n)) {
	Agedge_t *nxt;
	for (Agedge_t *e = agfstedge(maze, n); NULL != e; e = nxt) {
	  nxt = agnxtedge(maze, e, n);
	  if (NULL == aggetrec(e, "keep", 0)) {
		agdeledge(maze, e);
	  }
	}
  }
  
  free(stack);
}
