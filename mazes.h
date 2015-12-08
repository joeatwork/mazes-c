#ifndef __MAZES_H__
#define __MAZES_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct mazes_cell {
  size_t* location;
  size_t neighbors_length;
  size_t links_length;
  /* cell_number is guaranteed to be compact, unique and start at
     zero, so always less than mazes_maze_size(m). Should be suitable for
     indexing an array. */
  size_t cell_number;
  void *mark;
  struct mazes_cell **neighbors; // TODO may be NULL
  struct mazes_cell **links; // TODO may be NULL
  struct mazes_cell *next;
};

struct mazes_maze {
  /* dimensions * 2 must not overflow size_t */
  size_t dimensions;
  /* The product of all elements of size must not overflow size_t.
	 size[i] + 1 must not overflow size_t */
  size_t* size;
  struct mazes_cell *grid;
};

size_t mazes_maze_size(struct mazes_maze *maze);
struct mazes_cell *mazes_first_cell(struct mazes_maze *maze);
struct mazes_cell *mazes_random_cell(struct mazes_maze *maze);
struct mazes_cell *mazes_cell_at(
  struct mazes_maze *const maze,
  size_t *location
);
void mazes_maze_init(struct mazes_maze *maze, size_t *size, size_t size_length);
void mazes_maze_destroy(struct mazes_maze *maze);

void mazes_maze_arrange_as_rectangle(struct mazes_maze *maze);

/* cells must not link to themselves */
void mazes_cells_link(struct mazes_cell *from, struct mazes_cell *to);
void mazes_cells_unlink(struct mazes_cell *from, struct mazes_cell *to);
bool mazes_cells_are_linked(struct mazes_cell *from, struct mazes_cell *to);
size_t mazes_grid_index(struct mazes_maze *maze, size_t *location);
void mazes_grid_location(struct mazes_maze *maze, size_t index, /* out */ size_t *location);

#endif
