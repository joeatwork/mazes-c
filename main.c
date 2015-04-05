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

typedef void (*algorithm_type)(struct mazes_maze *maze);
typedef void (*coloring_type)(unsigned int *distances, struct mazes_maze *maze, struct mazes_cell *start);
typedef void (*format_type)(struct mazes_maze *maze, unsigned int *colors, FILE *stream);

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
  fprintf(stderr, "    aldous_broder (could take a long time!)\n");
  fprintf(stderr, "    backtracker (the default)\n");
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
  algorithm_type algorithm = mazes_generate_backtracker;
  coloring_type coloring = NULL;
  format_type format = mazes_print;

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
	format = mazes_dot;
      } else if (0 == strcmp("png", optarg)) {
	format = mazes_png;
      } else if (0 == strcmp("text", optarg)) {
	format = mazes_print;
      } else {
	fprintf(stderr, "format must be one of \"png\", \"text\", or \"dot\"\n");
	usage(command_name);
	return 1;
      }
      break;
    case 'a':
      if (0 == strcmp("aldous_broder", optarg)) {
	algorithm = mazes_generate_aldous_broder;
      } else if (0 == strcmp("backtracker", optarg)) {
	algorithm = mazes_generate_backtracker;
      } else if (0 == strcmp("binary_tree", optarg)) {
	algorithm = mazes_generate_binary_tree;
      } else if (0 == strcmp("sidewinder", optarg)) {
	algorithm = mazes_generate_sidewinder;
      } else {
	fprintf(stderr, "algorithm not recognized\n");
	usage(command_name);
	return 1;
      }
      break;
    case 'c':
      if (0 == strcmp("distance", optarg)) {
	coloring = mazes_distances;
      } else if (0 == strcmp("none", optarg)) {
	coloring = NULL;
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
  algorithm(&maze);

  unsigned int *colors = NULL;
  if (NULL != coloring) {
    colors = checked_calloc(MAZE_SIZE(&maze), sizeof(unsigned int));
    coloring(colors, &maze, mazes_first_cell(&maze));
  }

  format(&maze, colors, stdout);
  mazes_maze_destroy(&maze);
  return 0;
}
