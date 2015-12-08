#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mazes.h"
#include "utils.h"

size_t mazes_grid_index(struct mazes_maze *maze, size_t *location) {
  size_t ret = 0;
  size_t coefficient = 1;
  for (size_t i = 0; i < maze->dimensions; i++) {
	ret = ret + location[i] * coefficient;
	coefficient = coefficient * maze->size[i];
  }
  
  return ret;
}

void mazes_grid_location(struct mazes_maze *maze, size_t index, /* out */ size_t *location) {
  size_t coefficient = 1;
  for (size_t i = 0; i < maze->dimensions; i++) {
	coefficient = coefficient * maze->size[i];
  }

  size_t remainder = index;
  if (maze->dimensions > 0) {
	size_t i = maze->dimensions - 1;
	while (true) {
	  coefficient = coefficient / maze->size[i];
	  location[i] = remainder / coefficient;
	  remainder = remainder % coefficient;
	  if (0 == i) {
		break;
	  } else {
		i--;
	  }
	}
  }
}

static void cell_init(
  struct mazes_maze *maze,
  struct mazes_cell *cell,
  size_t index
) {
  cell->location = checked_calloc(maze->dimensions, sizeof(size_t));
  mazes_grid_location(maze, index, cell->location);
  cell->cell_number = index;  
  
  cell->links_length = 0;
  cell->neighbors_length = 0;
  cell->neighbors = NULL;
  cell->links = NULL;
  cell->mark = NULL;
  cell->next = NULL;
}

static void cell_destroy(struct mazes_cell *cell) {
  free(cell->neighbors);
  free(cell->links);
  free(cell->location);
}

size_t mazes_maze_size(struct mazes_maze *maze) {
  // TODO how do you know this will never overflow?
  if (0 == maze->dimensions) {
    return 0;
  }

  size_t ret = 1;
  for (size_t i = 0; i < maze->dimensions; i++) {
    ret = ret * maze->size[i];
  }

  return ret;
}

void mazes_cells_link(struct mazes_cell *from, struct mazes_cell *to) {
  assert(from != to);
  size_t new_length = from->links_length + 1;
  if (new_length > from->neighbors_length) {
    ERROR_EXIT("Too many links on a single maze cell (%lu neigbors, %lu attempted links)",
			   from->neighbors_length, new_length);
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
  if (0 == maze->dimensions) {
    return NULL;
  }
  
  for (size_t i = 0; i < maze->dimensions; i++) {
    if (0 == maze->size[i]) {
      return NULL;
    }
  }

  return maze->grid;
}

struct mazes_cell *mazes_random_cell(struct mazes_maze *maze) {
  size_t size = mazes_maze_size(maze);
  size_t index;
  RANDOM_CHOICE(index, size);
  return maze->grid + index;
}

struct mazes_cell *mazes_cell_at(
  struct mazes_maze *maze,
  size_t *location
) {
  size_t index = mazes_grid_index(maze, location);
  return &(maze->grid[index]);
}

void mazes_maze_arrange_as_rectangle(struct mazes_maze *maze) {
  size_t *neighbor_location = checked_calloc(maze->dimensions, sizeof(size_t));
  size_t neighbors_length = maze->dimensions * 2;
  
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
	cell->neighbors_length = neighbors_length;
	cell->links_length = 0;
	cell->neighbors = checked_calloc(cell->neighbors_length, sizeof(struct mazes_cell *));
	cell->links = checked_calloc(cell->neighbors_length, sizeof(struct mazes_cell *));
	memcpy(neighbor_location, cell->location, maze->dimensions * sizeof(size_t));

	for (size_t dim_i = 0; dim_i < maze->dimensions; dim_i++) {
	  size_t home_location = cell->location[dim_i];
	  if (home_location == 0) {
		cell->neighbors[dim_i * 2] = NULL;
	  } else {
		size_t neighbor = home_location - 1;
		neighbor_location[dim_i] = neighbor;
		cell->neighbors[dim_i * 2] = mazes_cell_at(maze, neighbor_location);
		neighbor_location[dim_i] = home_location;
	  }
	  
	  if (home_location == maze->size[dim_i] - 1) {
		cell->neighbors[dim_i * 2 + 1] = NULL;
	  } else {
		size_t neighbor = home_location + 1;
		neighbor_location[dim_i] = neighbor;
		cell->neighbors[dim_i * 2 + 1] = mazes_cell_at(maze, neighbor_location);
		neighbor_location[dim_i] = home_location;		
	  }
	}
  }

  free(neighbor_location);
}

void mazes_maze_init(
  struct mazes_maze *maze,
  size_t *size,
  size_t dimensions
) {
  maze->dimensions = dimensions;
  maze->size = checked_calloc(maze->dimensions, sizeof(size_t));
  memcpy(maze->size, size, dimensions * sizeof(size_t));
  
  size_t grid_size = mazes_maze_size(maze);
  maze->grid = checked_calloc(grid_size, sizeof(struct mazes_cell));

  size_t cell_count = mazes_maze_size(maze);
  for (size_t i = 0; i < cell_count; i++) {
	struct mazes_cell *cell = &(maze->grid[i]);
	cell_init(maze, cell, i);
	if (i < cell_count - 1) {
	  cell->next = &(maze->grid[i + 1]);
	}
  }
}

void mazes_maze_destroy(struct mazes_maze *maze) {
  size_t cell_count = mazes_maze_size(maze);
  for (size_t i = 0; i < cell_count; i++) {
    cell_destroy(&maze->grid[i]);
  }
  free(maze->size);
  free(maze->grid); // TODO leaks!
}
