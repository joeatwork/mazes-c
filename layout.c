#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "utils.h"

struct compare_state {
  struct maze_pt3 p;
  Agraph_t *maze;
};

// Assumes locations have been checked for overflow elsewhere
static int compare_nodes(const void *a, const void *b, void *state_ptr) {
  Agnode_t *a_node = *((Agnode_t **)a);
  Agnode_t *b_node = *((Agnode_t **)b);
  struct maze_pt3 a_pos, b_pos;
  struct compare_state *st = state_ptr;
  if (0 != maze_read_location(st->maze, a_node, &a_pos)) {
	ERROR_EXIT("only nodes with locations can be compared");
  }
  if (0 != maze_read_location(st->maze, b_node, &b_pos)) {
	ERROR_EXIT("only nodes with locations can be compared");
  }
  
  long a_measure = a_pos.x + (a_pos.y * st->p.x) + (a_pos.z * st->p.x * st->p.y);
  long b_measure = b_pos.x + (b_pos.y * st->p.x) + (b_pos.z * st->p.x * st->p.y);
  long result = a_measure - b_measure;

  if (result > 0) {
	return 1;
  } else if (result < 0) {
	return -1;
  }

  return 0;
}

int maze_read_location(Agraph_t *maze, Agnode_t *n, struct maze_pt3 *position) {
  size_t x = 0;
  size_t y = 0;
  size_t z = 0;
  bool x_valid = false;
  bool y_valid = false;
  bool z_valid = false;
  
  {
	char *xstr = agget(n, "_0");
	if (NULL != xstr) {
	  char *endptr;
	  long xl = strtol(xstr, &endptr, 10);
	  if (endptr != xstr && '\0' == *endptr && xl >= 0 && xl <= SIZE_MAX) {
		x = xl;
		x_valid = true;
	  }
	}
  }

  {
	char *ystr = agget(n, "_1");
	if (NULL != ystr) {
	  char *endptr;
	  long yl = strtol(ystr, &endptr, 10);
	  if (endptr != ystr && '\0' == *endptr && yl >= 0 && yl <= SIZE_MAX) {
		y = yl;
		y_valid = true;
	  }
	}
  }

  {
	char *zstr = agget(n, "_2");
	if (NULL != zstr) {
	  char *endptr;
	  long zl = strtol(zstr, &endptr, 10);
	  if (endptr != zstr && '\0' == *endptr && zl >= 0 && zl <= SIZE_MAX) {
		z = zl;
		z_valid = true;
	  }
	}
  }	

  if (x_valid && y_valid && z_valid) {
	position->x = x;
	position->y = y;
	position->z = z;
	return 0;
  }

  return 1;
}

struct maze_grid maze_read_grid(Agraph_t *maze) {
  size_t max_nodes_count = agnnodes(maze);
  Agnode_t **nodes = checked_calloc(max_nodes_count, sizeof(Agnode_t *));

  struct maze_pt3 max_dimensions = {0,0,0};
  size_t nodes_count = 0;
  for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze, n)) {
	struct maze_pt3 position;
	if (0 == maze_read_location(maze, n, &position)) {
	  if (max_dimensions.x < position.x)
		max_dimensions.x = position.x;
	  
	  if (max_dimensions.y < position.y)
		max_dimensions.y = position.y;

	  if (max_dimensions.z < position.z)
		max_dimensions.z = position.z;
	  
	  nodes[nodes_count] = n;
	  nodes_count++;
	}
  }

  struct maze_grid ret;
  ret.size.x = 0;
  ret.size.y = 0;
  ret.size.z = 0;
  ret.nodes_count = 0;
  ret.nodes = nodes;

  if (0 == nodes_count) {
	
  } else {
	if (ULONG_MAX == max_dimensions.x ||
		ULONG_MAX == max_dimensions.y ||
		ULONG_MAX == max_dimensions.z) {
	  ERROR_EXIT("dimensions too large");
	}
	
	max_dimensions.x++;
	max_dimensions.y++;
	max_dimensions.z++;

	struct compare_state st;
	st.p.x = max_dimensions.x;
	st.p.y = max_dimensions.y;
	st.p.z = max_dimensions.z;
	st.maze = maze;

	qsort_r(nodes, nodes_count, sizeof(Agnode_t *), compare_nodes, &st);

	ret.size.x = max_dimensions.x;
	ret.size.y = max_dimensions.y;
	ret.size.z = max_dimensions.z;
	ret.nodes_count = nodes_count;
	ret.nodes = nodes;
  }

  return ret;
}

void maze_destroy_grid(struct maze_grid grid) {
  free(grid.nodes);
}
