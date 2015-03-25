#include <stdlib.h>
#include "binary_tree.h"

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
      /* THIS IS A TOY. Randomness is a profound topic, but for the
	 application at hand (generating fun Mazes) what follows is
	 random and unbiased enough. */
      long int coin = rand() - (RAND_MAX/2);
      if (coin > 0) {
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
