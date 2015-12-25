#include <cgraph.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithms.h"
#include "utils.h"


void usage(char *command_name) {
  fprintf(stderr, "usage: %s --algorithm=<value>\n", command_name);
  fprintf(stderr, "reads a dot grid on stdin and turns it into a maze on stdout\n");
  fprintf(stderr, "available algorithms are:\n");
  fprintf(stderr, "    backtracker\n");
}

int main(int argc, char** argv) {
  char *command_name = argv[0];
  void (*algorithm)(Agraph_t *) = NULL;
  long seed = 12345;
  
  struct option options[] = {
    {.name = "algorithm", .has_arg = required_argument, .flag = NULL, .val = 'a'},
	{.name = "seed", .has_arg = required_argument, .flag = NULL, .val = 's'},
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  int opt;
  while (-1 != (opt = getopt_long(argc, argv, "", options, NULL))) {
    switch (opt) {
    case 'a':
	  if (0 == strcmp("backtracker", optarg)) {
        algorithm = mazes_generate_backtracker;
	  } else {
		fprintf(stderr, "algorithm not recognized\n");
        usage(command_name);
        return 1;
	  }
	  break;
	case 's':
      seed = strtol(optarg, NULL, 10);
      break;
    default:
      fprintf(stderr, "unrecognized option \'%c\'\n", opt);
      usage(command_name);
      return 1;
    }
  }

  if (NULL == algorithm) {
      fprintf(stderr, "algorithm is a required option\n");
      usage(command_name);
      return 1;
  }

  if (seed <= 0) {
	fprintf(stderr, "seed must be a number greater than zero\n");
	usage(command_name);
	return 1;
  }

  srand(seed);
  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }

  algorithm(maze);
  
  agwrite(maze, stdout);
  agclose(maze);
}
