#ifndef __DISTANCES_H__
#define __DISTANCES_H__

#include "mazes.h"

/* distances must be at least mazes_maze_size(maze) long, and will be
   indexed by cell->cell_number */
void mazes_distances(
  unsigned int *distances,
  struct mazes_maze *maze,
  struct mazes_cell *start
);

#endif
