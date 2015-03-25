#include <stdlib.h>
#include <string.h>
#include "mazes.h"
#include "utils.h"

static void mazes_cell_init(struct mazes_cell *cell, const size_t row, const size_t column) {
  cell->row = row;
  cell->column = column;
  cell->neighbors_length = NEIGHBORS_BUFFER_SIZE;
  cell->links_length = 0;
  for (size_t i = 0; i < NEIGHBORS_BUFFER_SIZE; i++) {
    cell->neighbors[i] = NULL;
  }
  cell->next = NULL;
}

void mazes_cells_link(struct mazes_cell *from, struct mazes_cell *to) {
  size_t new_length = from->links_length + 1;
  if (new_length > LINKS_BUFFER_SIZE) {
    ERROR_EXIT("Too many links on a single maze cell");
  }

  from->links[from->links_length] = to;
  from->links_length = new_length;
}

void mazes_cells_unlink(struct mazes_cell *from, struct mazes_cell *to) {
  size_t i;
  for (i = 0; i < from->links_length; i++) {
    if (to == from->links[i]) {
      break;
    }
  }

  if (i == from->links_length) {
    ERROR_EXIT("Can't unlink two cells that are already unlinked");
  }

  memmove(
    from->links + i,
    from->links + i + 1,
    from->links_length - (i + 1)
  );

  from->links_length = from->links_length - 1;
}

bool mazes_cells_are_linked(struct mazes_cell *from, struct mazes_cell *to) {
  for (size_t i = 0; i < from->links_length; i++) {
    if (to == from->links[i]) {
      return 1;
    }
  }

  return 0;
}

struct mazes_cell *mazes_first_cell(struct mazes_maze *maze) {
  if (0 == maze->row_count || 0 == maze->column_count) {
    return NULL;
  } else {
    return mazes_cell_at(maze, 0, 0);
  }
}

struct mazes_cell *mazes_cell_at(/* TODO is this still needed? */
  struct mazes_maze *maze,
  const size_t row,
  const size_t column
) {
  size_t grid_index = (row * maze->column_count) + column;
  return &(maze->grid[grid_index]);
}

void mazes_maze_init(struct mazes_maze *maze, const size_t row_count, const size_t column_count) {
  if (row_count > MAX_GRID_DIMENSION) {
    ERROR_EXIT("Mazes must have less than MAX_GRID_DIMENSION rows");
  }

  if (column_count > MAX_GRID_DIMENSION) {
    ERROR_EXIT("Mazes must have less than MAX_GRID_DIMENSION columns");
  }

  maze->row_count = row_count;
  maze->column_count = column_count;
  size_t grid_size = row_count * column_count;
  maze->grid = checked_calloc(grid_size, sizeof(struct mazes_cell));
  for (size_t row = 0; row < maze->row_count; row++) {
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *cell = mazes_cell_at(maze, row, col);
      mazes_cell_init(cell, row, col);
    }
  }

  /* rectangular neighborhoods -> NORTH, SOUTH, EAST, WEST */
  for (size_t row = 0; row < maze->row_count; row++) {
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *const cell = mazes_cell_at(maze, row, col);
      if (row > 0) {
	cell->neighbors[NORTH_NEIGHBOR] = mazes_cell_at(maze, row - 1, col);
      }

      if (row < maze->row_count - 1) {
	cell->neighbors[SOUTH_NEIGHBOR] = mazes_cell_at(maze, row + 1, col);
      }

      if (col > 0) {
	cell->neighbors[WEST_NEIGHBOR] = mazes_cell_at(maze, row, col - 1);
      }

      if (col < maze->column_count - 1) {
	cell->neighbors[EAST_NEIGHBOR] = mazes_cell_at(maze, row, col + 1);
      }

      size_t next_row = row;
      size_t next_col = col + 1;
      if (next_col >= maze->column_count) {
	next_col = 0;
	next_row = next_row + 1;
      }

      if (next_row < maze->row_count) {
	cell->next = mazes_cell_at(maze, next_row, next_col);
      }
    }
  }  
}

void mazes_maze_destroy(struct mazes_maze *maze) {
  free(maze->grid);
}
