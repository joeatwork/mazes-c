#include <graphviz/cgraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "utils.h"

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s\n", command_name);
  fprintf(stderr, "reads a dot grid on stdin and prints an ascii maze on stdout\n");
  fprintf(stderr, "expects input to have _0, _1, _2 attributes as created by 'grid'\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "It doesn't really work for mazes with gaps. Use png or scad for those.\n");
}

static void print_grid(Agraph_t *maze, struct maze_grid grid, FILE *stream) {
  long floor = 0;
  long row = -1;

  bool *south_paths = checked_calloc(grid.size.x, sizeof(bool));
  for (size_t i = 0; i < grid.size.x; i++) {
	south_paths[i] = false;
  }

  for (size_t i = 0; i < grid.nodes_count; i++) {
	Agnode_t *node = grid.nodes[i];
	struct maze_pt3 position;
	if (0 != maze_read_location(maze, node, &position)) {
	  ERROR_EXIT("grid must only contain locations");
	}

	if (position.z != floor) {
	  fputc('\n', stream);
	  fputc('+', stream);
	  for (size_t i = 0; i < grid.size.x; i++) {
		fputs("--+", stream);
	  }
	  fputc('\n', stream);

	  for (size_t i = 0; i < grid.size.x; i++) {
		south_paths[i] = false;
	  }
	  row = -1;
	  floor = position.z;
	}

	if (position.y != row) {
	  fputc('\n', stream);
	  fputc('+', stream);
	  for (size_t col = 0; col < grid.size.x; col++) {
		if (south_paths[col]) {
		  fputs("  ", stream);
		} else {
		  fputs("--", stream);
		}

		fputs("+", stream);
	  }
	  fputc('\n', stream);

	  fputc('|', stream);
	  row = position.y;
	}

	south_paths[position.x] = false;
	bool east_wall = false;
	bool up_wall = false;
	bool down_wall = false;
	for (Agedge_t *e = agfstedge(maze, node); NULL != e; e = agnxtedge(maze, e, node)) {
	  Agnode_t *other;
	  struct maze_pt3 other_position;
	  other = e->node;
	  if (0 == maze_read_location(maze, other, &other_position)) {
		if (other_position.y > position.y) {
		  south_paths[position.x] = true;
		}
		if (other_position.x > position.x) {
		  east_wall = true;
		}
		if (other_position.z > position.z) {
		  up_wall = true;
		}
		if (other_position.z < position.z) {
		  down_wall = true;
		}
	  }
	}

	if (up_wall && down_wall) {
	  fputs("x ", stream);
	} else if (up_wall) {
	  fputs("/ ", stream);
	} else if (down_wall) {
	  fputs("\\ ", stream);
	} else {
	  fputs("  ", stream);
	}

	if (east_wall) {
	  fputc(' ', stream);
	} else {
	  fputc('|', stream);
	}
  }

  fputc('\n', stream);
  fputc('+', stream);
  for (size_t i = 0; i < grid.size.x; i++) {
	fputs("--+", stream);
  }
  fputc('\n', stream);
  free(south_paths);
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
  print_grid(maze, grid, stdout);
  maze_destroy_grid(grid);
  agclose(maze);
}
