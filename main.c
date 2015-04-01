#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms.h"
#include "distances.h"
#include "dotter.h"
#include "mazes.h"
#include "pnger.h"
#include "printer.h"
#include "utils.h"

enum format_type {
  FORMAT_TYPE_TEXT,
  FORMAT_TYPE_PNG,
  FORMAT_TYPE_DOT,
};

enum algorithm_type {
  ALGORITHM_TYPE_BINARY_TREE,
  ALGORITHM_TYPE_SIDEWINDER,
};

enum coloring_type {
  COLORING_TYPE_DISTANCE,
  COLORING_TYPE_NONE,
};

void usage(char *command_name) {
  fprintf(stderr, "usage: %s --width=<value> --height=<value> [--format=format]\n", command_name);
  fprintf(stderr, "          [--format=format] [--algorithm=algorithm] [--coloring=coloring]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "available formats are:\n");
  fprintf(stderr, "    dot      produce the maze as a dot file\n");
  fprintf(stderr, "    png      produce the maze as a png file\n");
  fprintf(stderr, "    text     print the maze as text\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "available algorithms are:\n");
  fprintf(stderr, "    binary_tree\n");
  fprintf(stderr, "    sidewinder\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "available colorings are:\n");
  fprintf(stderr, "    distance\n");
  fprintf(stderr, "\n");
}

int main(int argc, char** argv) {
  struct mazes_maze maze;
  char *command_name = argv[0];

  struct option options[] = {
    {.name = "width", .has_arg = required_argument, .flag = NULL, .val = 'w'},
    {.name = "height", .has_arg = required_argument, .flag = NULL, .val = 'h'},
    {.name = "format", .has_arg = required_argument, .flag = NULL, .val = 'f'},
    {.name = "algorithm", .has_arg = required_argument, .flag = NULL, .val = 'a'},
    {.name = "coloring", .has_arg = required_argument, .flag = NULL, .val = 'c'},
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  long propose_width = 0;
  long propose_height = 0;
  enum format_type format = FORMAT_TYPE_TEXT;
  enum algorithm_type algorithm = ALGORITHM_TYPE_BINARY_TREE;
  enum coloring_type coloring = COLORING_TYPE_NONE;

  int opt;
  while (-1 != (opt = getopt_long(argc, argv, "w:h:f:a:", options, NULL))) {
    switch (opt) {
    case 'w':
      propose_width = strtol(optarg, NULL, 10);
      break;
    case 'h':
      propose_height = strtol(optarg, NULL, 10);
      break;
    case 'f':
      if (0 == strcmp("dot", optarg)) {
	format = FORMAT_TYPE_DOT;
      } else if (0 == strcmp("png", optarg)) {
	format = FORMAT_TYPE_PNG;
      } else if (0 == strcmp("text", optarg)) {
	format = FORMAT_TYPE_TEXT;
      } else {
	fprintf(stderr, "format must be one of \"png\" or \"text\"\n");
	usage(command_name);
	return 1;
      }
      break;
    case 'a':
      if (0 == strcmp("binary_tree", optarg)) {
	algorithm = ALGORITHM_TYPE_BINARY_TREE;
      } else if (0 == strcmp("sidewinder", optarg)) {
	algorithm = ALGORITHM_TYPE_SIDEWINDER;
      } else {
	fprintf(stderr, "algorithm must be \"binary_tree\" or \"sidewinder\"\n");
	usage(command_name);
	return 1;
      }
      break;
    case 'c':
      if (0 == strcmp("distance", optarg)) {
	coloring = COLORING_TYPE_DISTANCE;
      } else if (0 == strcmp("none", optarg)) {
	coloring = COLORING_TYPE_NONE;
      } else {
	fprintf(stderr, "coloring must be \"distance\" or \"none\"\n");
	usage(command_name);
	return 1;
      }
      break;
    default:
      fprintf(stderr, "unrecognized option \'%c\'\n", opt);
      usage(command_name);
      return 1;
    }
  }

  if (propose_width < 1 ||
      propose_height < 1 ||
      propose_width > MAX_GRID_DIMENSION ||
      propose_height > MAX_GRID_DIMENSION) {
    fprintf(stderr, "Width and height must be greater than 0 and less than or equal to %d\n", MAX_GRID_DIMENSION);
    usage(command_name);
    return 1;
  }

  mazes_maze_init(&maze, (size_t) propose_width, (size_t) propose_height);
  switch (algorithm) {
  case ALGORITHM_TYPE_BINARY_TREE:
    mazes_generate_binary_tree(&maze);
    break;
  case ALGORITHM_TYPE_SIDEWINDER:
    mazes_generate_sidewinder(&maze);
    break;
  default:
    ERROR_EXIT("Unrecognized algorithm %d", algorithm);
  }

  unsigned int *colors = NULL;
  switch (coloring) {
  case COLORING_TYPE_NONE:
    break;
  case COLORING_TYPE_DISTANCE:
    colors = checked_calloc(MAZE_SIZE(&maze), sizeof(unsigned int));
    mazes_distances(colors, &maze, mazes_first_cell(&maze));
    break;
  default:
    ERROR_EXIT("Unrecognized coloring %d", coloring);
  }

  switch (format) {
  case FORMAT_TYPE_DOT:
    mazes_dot(&maze, colors, stdout);
    break;
  case FORMAT_TYPE_PNG:
    mazes_png(&maze, colors, stdout);
    break;
  case FORMAT_TYPE_TEXT:
    mazes_fprint(&maze, colors, stdout);
    break;
  default:
    ERROR_EXIT("Unrecognized output format %d", format);
  }

  mazes_maze_destroy(&maze);
  return 0;
}
