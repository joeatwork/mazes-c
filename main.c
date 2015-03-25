#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary_tree.h"
#include "mazes.h"
#include "pnger.h"
#include "printer.h"
#include "utils.h"

enum format_type {
  FORMAT_TYPE_TEXT,
  FORMAT_TYPE_PNG,
};

enum algorithm_type {
  ALGORITHM_TYPE_BINARY_TREE,
};

void usage(char *command_name) {
  fprintf(stderr, "usage: %s --width=<value> --height=<value> [--format=format]\n", command_name);
  fprintf(stderr, "          [--format=format] [--algorithm=algorithm]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "available formats are:\n");
  fprintf(stderr, "    text     print the maze as text\n");
  fprintf(stderr, "    png      produce the maze as a png file\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "available algorithms are:\n");
  fprintf(stderr, "    binary_tree\n");
}

int main(int argc, char** argv) {
  struct mazes_maze maze;
  char *command_name = argv[0];

  struct option options[] = {
    {.name = "width", .has_arg = required_argument, .flag = NULL, .val = 'w'},
    {.name = "height", .has_arg = required_argument, .flag = NULL, .val = 'h'},
    {.name = "format", .has_arg = required_argument, .flag = NULL, .val = 'f'},
    {.name = "algorithm", .has_arg = required_argument, .flag = NULL, .val = 'a'},
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  long propose_width = 0;
  long propose_height = 0;
  enum format_type format = FORMAT_TYPE_TEXT;
  enum algorithm_type algorithm = ALGORITHM_TYPE_BINARY_TREE;

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
      if (0 == strcmp("png", optarg)) {
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
      } else {
	fprintf(stderr, "algorithm must be \"binary_tree\"\n");
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
  default:
    ERROR_EXIT("Unrecognized algorithm %d", algorithm);
  }

  switch (format) {
  case FORMAT_TYPE_TEXT:
    mazes_fprint(&maze, stdout);
    break;
  case FORMAT_TYPE_PNG:
    mazes_png(&maze, stdout);
    break;
  default:
    ERROR_EXIT("Unrecognized output format %d", format);
  }

  mazes_maze_destroy(&maze);
  return 0;
}
