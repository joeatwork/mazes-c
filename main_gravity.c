#include <graphviz/cgraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "utils.h"

void usage(char *command_name) {
  fprintf(stderr, "usage: %s\n", command_name);
  fprintf(stderr, "Adds edges to a graph so it will work on a 3d printer\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "gravity will attempt to add an edge to any node in the");
  fprintf(stderr, "graph given on stdin that doesn't already have an edge");
  fprintf(stderr, "connecting it to a neighbor in the negative x, y, or z");
  fprintf(stderr, "direction. That means that if you try to print a graph");
  fprintf(stderr, "processed by gravity on a 3d printer, and that graph");
  fprintf(stderr, "is balanced on it's 0,0,0th node before printing, it'll");
  fprintf(stderr, "be more likely to make a good print without needing a lot");
  fprintf(stderr, "of support.");
}

#define MAX_NEIGHBOR_LENGTH 3
long lower_neighbors[MAX_NEIGHBOR_LENGTH][3] = {
  {-1, 0, 0},
  { 0,-1, 0},
  { 0, 0,-1},
};

void add_gravity_edges(Agraph_t *maze, struct maze_grid grid) {
  for (int i = 0; i < grid.nodes_count; i++) {
	Agnode_t *node = grid.nodes[i];

	struct maze_pt3 self_location;
	if (0 != maze_read_location(maze, node, &self_location)) {
	  continue;
	}

	size_t candidates_count = 0;
	Agnode_t *candidates[MAX_NEIGHBOR_LENGTH];
	bool supported = false;
	
	for (size_t i = 0; i < MAX_NEIGHBOR_LENGTH && !supported; i++) {
	  if (self_location.x < -lower_neighbors[i][0]) {
		continue;
	  }

	  if (self_location.y < -lower_neighbors[i][1]) {
		continue;
	  }

	  if (self_location.z < -lower_neighbors[i][2]) {
		continue;
	  }

	  struct maze_pt3 try = {
		.x = self_location.x + lower_neighbors[i][0],
		.y = self_location.y + lower_neighbors[i][1],
		.z = self_location.z + lower_neighbors[i][2]
	  };

	  Agnode_t *candidate;
	  if (NULL != (candidate = maze_find_in_grid_at_pt(maze, grid, try))) {
		candidates[candidates_count] = candidate;
		candidates_count++;

		if (NULL != agedge(maze, node, candidate, NULL, false)) {
		  supported = true;
		}
	  }
	}

	if (!supported && candidates_count > 0) {
	  size_t pick;
	  RANDOM_CHOICE(pick, candidates_count);
	  Agnode_t *support = candidates[pick];
	  if (NULL != support) {
		Agedge_t *added = agedge(maze, node, support, NULL, 1);
		agsafeset(added, "gravity_support", "true", "");
	  }
	}
  }//for
}

int main(int argc, char** argv) {
  char *command_name = argv[0];
  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }

  struct maze_grid grid = maze_read_grid(maze);
  add_gravity_edges(maze, grid);
  agwrite(maze, stdout);
  maze_destroy_grid(grid);  
  
  agclose(maze);
}
