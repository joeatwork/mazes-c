#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "utils.h"

/* THIS IS A TOY. Randomness is a profound topic, but for the
   application at hand (generating fun Mazes) what follows is
   random and unbiased enough. */
#define RANDOM_CHOICE(name, n)\
  do { name = rand() / (RAND_MAX/n); } while (name == n && n != 0)

void mazes_generate_binary_tree(struct mazes_maze *maze) {
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    struct mazes_cell *southern = cell->neighbors[SOUTH_NEIGHBOR];
    struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
    struct mazes_cell *link;

    if (NULL == southern && NULL == eastern) {
      link = NULL;
    } else if (NULL == southern) {
      link = eastern;
    } else if (NULL == eastern) {
      link = southern;
    } else {
      int coin;
      RANDOM_CHOICE(coin, 2);
      if (coin) {
	link = southern;
      } else {
	link = eastern;
      }
    }

    if (NULL != link) {
      mazes_cells_link(cell, link);
      mazes_cells_link(link, cell);
    }
  }
}

void mazes_generate_sidewinder(struct mazes_maze *maze) {
  static struct mazes_cell *run_buffer[MAX_GRID_DIMENSION];
  for (size_t row = 0; row < maze->row_count; row++) {
    size_t run_length = 0;
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *cell = mazes_cell_at(maze, row, col);
      run_buffer[run_length] = cell;
      run_length++;

      struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
      int coin;
      RANDOM_CHOICE(coin, 2);
      if (NULL == eastern || (NULL != cell->neighbors[SOUTH_NEIGHBOR] && coin)) {
	size_t choice_ix;
	RANDOM_CHOICE(choice_ix, run_length);
	struct mazes_cell *choice = run_buffer[choice_ix];
	struct mazes_cell *choice_southern = choice->neighbors[SOUTH_NEIGHBOR];

	if (NULL != choice_southern) {
	  mazes_cells_link(choice, choice_southern);
	  mazes_cells_link(choice_southern, choice);
	}

	run_length = 0;
      } else {
	mazes_cells_link(cell, eastern);
	mazes_cells_link(eastern, cell);
      }
    }
  }
}
