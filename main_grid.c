#include <cgraph.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void usage(char *command_name) {
  fprintf(stderr, "usage: %s --width=<value> [--height=<value> [--depth=<value>]]\n", command_name);
  fprintf(stderr, "Produces a dot grid of unconnected nodes to stdout\n");
}

int main(int argc, char** argv) {
  char *command_name = argv[0];

  struct option options[] = {
    {.name = "width", .has_arg = required_argument, .flag = NULL, .val = 'w'},
    {.name = "height", .has_arg = required_argument, .flag = NULL, .val = 'h'},
	{.name = "depth", .has_arg = required_argument, .flag = NULL, .val = 'd'},
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  long propose_width = 1;
  long propose_height = 1;
  long propose_depth = 1;

  int opt;
  while (-1 != (opt = getopt_long(argc, argv, "", options, NULL))) {
    switch (opt) {
    case 'w':
      propose_width = strtol(optarg, NULL, 10);
      break;
    case 'h':
      propose_height = strtol(optarg, NULL, 10);
      break;
    case 'd':
      propose_depth = strtol(optarg, NULL, 10);
      break;
    default:
      fprintf(stderr, "unrecognized option \'%c\'\n", opt);
      usage(command_name);
      return 1;
    }
  }

  if (propose_width < 0 ||
	  propose_height < 0 ||
	  propose_depth < 0) {
	fprintf(stderr, "width, depth, and height must all be greater than or equal to 0 if present\n");
	usage(command_name);
	return 1;
  }

  char label[256];
  char attrvalue[12];
  Agraph_t *maze = agopen("maze", Agstrictundirected, NULL);
  if (NULL == maze) {
	ERROR_EXIT("Cannot allocate new maze");
  }
  
  for (long x = 0; x < propose_width; x++) {
	for (long y = 0; y < propose_height; y++) {
	  for (long z = 0; z < propose_depth; z++) {
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
	}
  }

  for (long x = 0; x < propose_width; x++) {
	for (long y = 0; y < propose_height; y++) {
	  for (long z = 0; z < propose_depth; z++) {
		checked_snprintf(label, sizeof(label), "node_%ld_%ld_%ld", x, y, z);
		Agnode_t *source = agnode(maze, label, false);
		if (NULL == source) {
		  ERROR_EXIT("Can't find a graph node");
		}

		if (x < propose_width - 1) {
		  checked_snprintf(label, sizeof(label), "node_%ld_%ld_%ld", x + 1, y, z);
		  Agnode_t *dest = agnode(maze, label, false);
		  if (NULL == dest) {
			ERROR_EXIT("Can't find a graph node");
		  }
		  agedge(maze, source, dest, NULL, 1); 
		}

		if (y < propose_height - 1) {
		  checked_snprintf(label, sizeof(label), "node_%ld_%ld_%ld", x, y + 1, z);
		  Agnode_t *dest = agnode(maze, label, false);
		  if (NULL == dest) {
			ERROR_EXIT("Can't find a graph node");
		  }
		  agedge(maze, source, dest, NULL, 1); 				  
		}

		if (z < propose_depth - 1) {
		  checked_snprintf(label, sizeof(label), "node_%ld_%ld_%ld", x, y, z + 1);
		  Agnode_t *dest = agnode(maze, label, false);
		  if (NULL == dest) {
			ERROR_EXIT("Can't find a graph node");
		  }
		  agedge(maze, source, dest, NULL, 1);
		}
	  }
	}
  }
  
  agwrite(maze, stdout);
  agclose(maze);
}
