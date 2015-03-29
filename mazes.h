#ifndef __MAZES_H__
#define __MAZES_H__

#include <stdbool.h>

/* We don't check for overflow in grids,
   so MAX_GRID_DIMENSION must be < sqrt(SIZE_MAX)
   and MAX_GRID_DIMENSION must be < RAND_MAX
   and (for PNGs) MAX_GRID_DIMENSION < INT_MAX * SOME LOW CONSTANT (see pnger.c) */
#define MAX_GRID_DIMENSION 128
#define LINKS_BUFFER_SIZE 4
#define NEIGHBORS_BUFFER_SIZE 4
#define NORTH_NEIGHBOR 0
#define SOUTH_NEIGHBOR 1
#define EAST_NEIGHBOR 2
#define WEST_NEIGHBOR 3

struct mazes_cell {
  size_t row;
  size_t column;
  size_t neighbors_length;
  size_t links_length;
  struct mazes_cell *neighbors[NEIGHBORS_BUFFER_SIZE];
  struct mazes_cell *links[LINKS_BUFFER_SIZE];
  struct mazes_cell *next;
};

struct mazes_maze {
  size_t row_count;
  size_t column_count;
  struct mazes_cell *grid;
};

struct mazes_iter;

struct mazes_cell *mazes_first_cell(struct mazes_maze *maze);
struct mazes_cell *mazes_cell_at(
  struct mazes_maze *const maze,
  const size_t row,
  const size_t column
);
void mazes_cells_link(struct mazes_cell *from, struct mazes_cell *to);
void mazes_cells_unlink(struct mazes_cell *from, struct mazes_cell *to);
bool mazes_cells_are_linked(struct mazes_cell *from, struct mazes_cell *to);
void mazes_maze_init(struct mazes_maze *maze, size_t row_count, size_t column_count);
void mazes_maze_destroy(struct mazes_maze *maze);

#endif
