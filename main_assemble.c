#include <graphviz/cgraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "layout.h"
#include "utils.h"

void usage(char *command_name) {
  fprintf(stderr, "usage: %s\n", command_name);
  fprintf(stderr, "Reads coordinates from stdin and adds edges between adjacent\n");
  fprintf(stderr, "rectangular coordinates into a shaped grid, and writes\n");
  fprintf(stderr, "a dot graph to stdout.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Input should be lines of three numbers, separated by commas, like:\n");
  fprintf(stderr, "1,2,3\n");
}

int main(int argc, char** argv) {
  char *command_name = argv[0];

  char label[256];
  char attrvalue[12];
  Agraph_t *maze = agopen("maze", Agstrictundirected, NULL);
  if (NULL == maze) {
	ERROR_EXIT("Cannot allocate new maze");
  }

  int max_x = 0;
  int max_y = 0;
  int max_z = 0;

  while (!feof(stdin)) {
	int x, y, z;
	int ret = fscanf(stdin, " %d , %d , %d ", &x, &y, &z);
	if (3 != ret) {
	  fprintf(stderr, "Stdin should be a set of comma separated number triples\n");
	  usage(command_name);
	  return 1;
	}

	if (x < 0 || y < 0 || z < 0) {
	  fprintf(stderr, "All coordinates must be greater than or equal to zero\n");
	  usage(command_name);
	  return 1;
	}

	if (INT_MAX == x || INT_MAX == y || INT_MAX == z) {
	  fprintf(stderr, "Coordinates are too large\n");
	  usage(command_name);
	  return 1;
	}

	if (max_x < x) {
	  max_x = x;
	}

	if (max_y < y) {
	  max_y = y;
	}

	if (max_z < z) {
	  max_z = z;
	}

	checked_snprintf(label, sizeof(label), "node_%ld_%ld_%ld", x, y, z);
	Agnode_t *node = agnode(maze, label, true);
	if (NULL == node) {
	  ERROR_EXIT("Can't allocate a graph node");
	}

	checked_snprintf(attrvalue, sizeof(attrvalue), "%ld", x);
	agsafeset(node, "_0", attrvalue, "");

	checked_snprintf(attrvalue, sizeof(attrvalue), "%ld", y);
	agsafeset(node, "_1", attrvalue, "");

	checked_snprintf(attrvalue, sizeof(attrvalue), "%ld", z);
	agsafeset(node, "_2", attrvalue, "");
  }

  max_x++;
  max_y++;
  max_z++;

  struct maze_grid grid = maze_read_grid(maze);
  for (size_t i = 0; i < grid.nodes_count; i++) {
	Agnode_t *node = grid.nodes[i];
	struct maze_pt3 node_pos;
	if (0 != maze_read_location(maze, node, &node_pos)) {
	  ERROR_EXIT("gridded nodes should always have locations");
	}

	struct maze_pt3 other_pos;
	Agnode_t *other;
	other_pos = node_pos;
	if (other_pos.x > 0) {
	  other_pos.x--;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}

	other_pos = node_pos;
	if (other_pos.y > 0) {
	  other_pos.y--;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}

	other_pos = node_pos;
	if (other_pos.z > 0) {
	  other_pos.z--;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}

	other_pos = node_pos;
	if (other_pos.x < max_x - 1) {
	  other_pos.x++;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}

	other_pos = node_pos;
	if (other_pos.y < max_y - 1) {
	  other_pos.y++;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}

	other_pos = node_pos;
	if (other_pos.z < max_z - 1) {
	  other_pos.z++;
	  if (NULL != (other = maze_find_in_grid_at_pt(maze,grid, other_pos))) {
		agedge(maze, node, other, NULL, 1);
	  }
	}
  }

  agwrite(maze, stdout);
  agclose(maze);
}
