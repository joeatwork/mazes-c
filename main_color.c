#include <graphviz/cgraph.h>
#include <math.h>
#include <getopt.h>
#include <stdbool.h>
#include "color.h"
#include "utils.h"

// Cargo-culted from a stack-overflow answer
// (in particular, http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both)
static struct maze_rgb hue2rgb(double hh) {
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    int i = (long)hh;
    double ff = hh - i;
    double p = 0;
    double q = 1.0 - ff;

    struct maze_rgb out;
    switch(i) {
    case 0:
        out.r = 1.0;
        out.g = ff;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = 1.0;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = 1.0;
        out.b = ff;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = 1.0;
        break;
    case 4:
        out.r = ff;
        out.g = p;
        out.b = 1.0;
        break;
    case 5:
    default:
        out.r = 1.0;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s [--seed=number]\n", command_name);
  fprintf(stderr, "Annotates the nodes on a graph with a distance from some random spot\n");
  fprintf(stderr, "reads stdin in dot format, writes to stdout\n");
}

struct distance_rec {
  Agrec_t header;
  size_t distance;
};

int main(int argc, char** argv) {
  char *command_name = argv[0];

  long seed = 12345;
  struct option options[] = {
	{.name = "seed", .has_arg = required_argument, .flag = NULL, .val = 's'},
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  int opt;
  while (-1 != (opt = getopt_long(argc, argv, "", options, NULL))) {
    switch (opt) {
	case 's':
      seed = strtol(optarg, NULL, 10);
      break;
	default:
      fprintf(stderr, "unrecognized option \'%c\'\n", opt);
      usage(command_name);
      return 1;
    }
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

  size_t nodecount = agnnodes(maze);
  if (nodecount > 0) {

	Agnode_t *found = NULL;
	{
	  size_t at = 0;
	  size_t pick;
	  RANDOM_CHOICE(pick, nodecount);
	  for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze, n)) {
		if (pick == at) {
		  found = n;
		  break;
		}
		at++;
	  }
	}

	if (NULL == found) {
	  ERROR_EXIT("failed to pick a starting node");
	}

	Agnode_t **stack = checked_calloc(nodecount, sizeof(Agnode_t *));
	stack[0] = found;
	size_t stack_size = 1;
	struct distance_rec *dist0 =
	  agbindrec(stack[0], "distance", sizeof(struct distance_rec), false);
	dist0->distance = 0;
	size_t max_distance = 0;
	while (stack_size > 0) {
	  Agnode_t *next = stack[stack_size - 1];
	  stack_size--;
	  
	  struct distance_rec *dist = (struct distance_rec *)aggetrec(next, "distance", false);
	  if (max_distance < dist->distance) {
		max_distance = dist->distance;
	  }
	  
	  size_t next_distance = dist->distance + 1;
	  for (Agedge_t *e = agfstedge(maze, next); NULL != e; e = agnxtedge(maze, e, next)) {
		Agnode_t *other = e->node;

		if (NULL == aggetrec(other, "distance", 0)) {
		  struct distance_rec *nxt =
			agbindrec(other, "distance", sizeof(struct distance_rec), false);
		  nxt->distance = next_distance;
		  stack[stack_size] = other;
		  stack_size++;
		}
	  }
	}
	free(stack);

	double start_hue = 360.0 * ((double) rand()/RAND_MAX);
	double end_hue = fmod(start_hue + 180.0, 360.0);
	struct maze_rgb start_color = hue2rgb(start_hue);
	struct maze_rgb end_color = hue2rgb(end_hue);
	struct maze_rgb increment;
	increment.r = (end_color.r - start_color.r)/(double)max_distance;
	increment.g = (end_color.g - start_color.g)/(double)max_distance;
	increment.b = (end_color.b - start_color.b)/(double)max_distance;

	for (Agnode_t *n = agfstnode(maze); NULL != n; n = agnxtnode(maze, n)) {
	  struct distance_rec *dist;
	  if (NULL != (dist = (struct distance_rec *)aggetrec(n, "distance", 0))) {
		char attrvalue[256];
		double r = start_color.r + (increment.r * dist->distance);
		double g = start_color.g + (increment.g * dist->distance);
		double b = start_color.b + (increment.b * dist->distance);
		
		checked_snprintf(attrvalue, sizeof(attrvalue), "%f,%f,%f", r, g, b);
		agsafeset(n, "color", attrvalue, "");
	  }
	}
  }

  agwrite(maze, stdout);
  agclose(maze);
}
