#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "utils.h"

void mazes_generate_tree(struct mazes_maze *maze) {
  // RECTANGULAR MAZES *ONLY*

  struct mazes_cell **candidates =
	checked_calloc(maze->dimensions, sizeof(struct mazes_cell *));
  
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
	// One positive neighbor for each dimension
	size_t candidates_length = 0;
	for (size_t dim = 0; dim < maze->dimensions; dim++) {
	  struct mazes_cell *candidate = cell->neighbors[dim * 2 + 1];
	  if (NULL != candidate) {
		candidates[candidates_length] = candidate;
		candidates_length++;
	  }
	}	

    struct mazes_cell *link = NULL;
	if (candidates_length > 0) {
	  size_t pick;
      RANDOM_CHOICE(pick, candidates_length);
	  link = candidates[pick];
    }

    if (NULL != link) {
      mazes_cells_link(cell, link);
      mazes_cells_link(link, cell);
    }
  }

  free(candidates);
}

void mazes_generate_aldous_broder(struct mazes_maze *maze) {
  struct mazes_cell *cell = mazes_random_cell(maze);
  size_t remaining = mazes_maze_size(maze) - 1;
  size_t neighbor_choice;
  while (remaining > 0) {
    RANDOM_CHOICE(neighbor_choice, cell->neighbors_length);
    struct mazes_cell *neighbor = cell->neighbors[neighbor_choice];
    if (NULL != neighbor) {
      if (0 == neighbor->links_length) {
        mazes_cells_link(cell, neighbor);
        mazes_cells_link(neighbor, cell);
        remaining = remaining - 1;
      }

      cell = neighbor;
    }
  } // while
}

void mazes_generate_backtracker(struct mazes_maze *maze) {
  size_t stack_max = mazes_maze_size(maze);
  struct mazes_cell** stack = checked_calloc(stack_max, sizeof(struct mazes_cell *));
  size_t stack_depth = 0;

  struct mazes_cell *start = mazes_random_cell(maze);
  stack[stack_depth] = start;
  stack_depth = stack_depth + 1;

  while (0 < stack_depth) {
    struct mazes_cell *stack_top = stack[stack_depth - 1];
    size_t candidate_count = 0;
    for (size_t i = 0; i < stack_top->neighbors_length; i++) {
      if (NULL != stack_top->neighbors[i] && 0 == stack_top->neighbors[i]->links_length) {
        candidate_count = candidate_count + 1;
      }
    }

    if (0 == candidate_count) {
      stack_depth = stack_depth - 1;
    } else {
      size_t next_ix;
      RANDOM_CHOICE(next_ix, candidate_count);
      size_t found = 0;
      struct mazes_cell *next_cell = NULL;
      for (size_t i = 0; i < stack_top->neighbors_length && NULL == next_cell; i++) {
        if (NULL != stack_top->neighbors[i] && 0 == stack_top->neighbors[i]->links_length) {
          if (found == next_ix) {
            next_cell = stack_top->neighbors[i];
          }

          found = found + 1;
        }
      }

      assert(NULL != next_cell);
      mazes_cells_link(stack_top, next_cell);
      mazes_cells_link(next_cell, stack_top);
      stack[stack_depth] = next_cell;
      stack_depth = stack_depth + 1;
    }
  }

  free(stack);
}
