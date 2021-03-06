#include <graphviz/cgraph.h>
#include <cairo/cairo.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "color.h"
#include "layout.h"
#include "utils.h"

#define MAX_PATH_WIDTH 1024
#define DEFAULT_PATH_WIDTH 10

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s [--paths=<bool> --walls=<bool> --path-width=<number>]\n", command_name);
  fprintf(stderr, "reads a dot grid on stdin and prints an ascii maze on stdout\n");
  fprintf(stderr, "expects input to have _0, _1, _2 attributes as created by 'grid'\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "By default, will print the walls of the maze, %d pixels apart.\n", DEFAULT_PATH_WIDTH);
  fprintf(stderr, "if --paths present and true will print a 1 pixel line along paths as well.\n");
  fprintf(stderr, "To render without walls, use --walls=false\n");
}

static cairo_status_t write_to_stream(void *closure, const unsigned char *data, unsigned int length) {
  FILE *stream = closure;
  fwrite(data, sizeof(unsigned char), length, stream);
  if (ferror(stream)) {
    ERROR_EXIT("Error writing to output file");
  }
  return CAIRO_STATUS_SUCCESS;
}

static void draw_paths(cairo_t *cairo, struct maze_pt3 pos, uint path_width, bool north, bool south, bool east, bool west) {
  double half = path_width/2.0;
  double center_x = (pos.x * path_width) + half;
  double center_y = (pos.y * path_width) + half;
  if (north) {
	cairo_move_to(cairo, center_x, pos.y * path_width);
	cairo_line_to(cairo, center_x, center_y);
	cairo_stroke(cairo);
  }

  if (south) {
	cairo_move_to(cairo, center_x, (pos.y + 1) * path_width);
	cairo_line_to(cairo, center_x, center_y);
	cairo_stroke(cairo);
  }

  if (west) {
	cairo_move_to(cairo, (pos.x + 1) * path_width, center_y);
	cairo_line_to(cairo, center_x, center_y);
	cairo_stroke(cairo);
  }

  if (east) {
	cairo_move_to(cairo, pos.x * path_width, center_y);
	cairo_line_to(cairo, center_x, center_y);
	cairo_stroke(cairo);
  }
}

static void draw_walls(cairo_t *cairo, struct maze_pt3 pos, uint path_width, bool north, bool south, bool east, bool west) {
  if (!north) {
	cairo_move_to(cairo, pos.x * path_width, pos.y * path_width);
	cairo_line_to(cairo, (pos.x + 1) * path_width, pos.y * path_width);
	cairo_stroke(cairo);
  }

  if (!south) {
	cairo_move_to(cairo, pos.x * path_width, (pos.y + 1) * path_width);
	cairo_line_to(cairo, (pos.x + 1) * path_width, (pos.y + 1) * path_width);
	cairo_stroke(cairo);
  }

  if (!east) {
	cairo_move_to(cairo, pos.x * path_width, pos.y * path_width);
	cairo_line_to(cairo, pos.x * path_width, (pos.y + 1) * path_width);
	cairo_stroke(cairo);
  }

  if (!west) {
	cairo_move_to(cairo, (pos.x + 1) * path_width, pos.y * path_width);
	cairo_line_to(cairo, (pos.x + 1) * path_width, (pos.y + 1) * path_width);
	cairo_stroke(cairo);
  }
}

static void png_grid(Agraph_t *maze, struct maze_grid grid, FILE *stream, uint path_width, bool do_walls, bool do_paths) {
  if (grid.size.x > (INT_MAX / (path_width + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  if (grid.size.y > (INT_MAX / (path_width + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  if (grid.size.z > (INT_MAX / (path_width + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  int width = grid.size.x * path_width;
  int height = grid.size.y * path_width;

  cairo_surface_t *surface = cairo_image_surface_create(
    CAIRO_FORMAT_RGB16_565,
    width + 1,
    height + 1
  );
  cairo_t *cairo = cairo_create(surface);
  cairo_rectangle(cairo, 0.0, 0.0, width + 1, height + 1);
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_fill(cairo);

  cairo_set_line_width(cairo, 1.0);

  for (size_t i = 0; i < grid.nodes_count; i++) {
	Agnode_t *node = grid.nodes[i];
	struct maze_rgb color;
	if (0 != maze_read_rgb(node, &color)) {
	  color.r = 1.0;
	  color.g = 1.0;
	  color.b = 1.0;
	}

	struct maze_pt3 pos;
	if (0 != maze_read_location(maze, node, &pos)) {
	  ERROR_EXIT("grid elements must all have locations");
	}

	if (pos.z > 0) {
	  break; // TODO for now, only render the bottom floor
	}

	cairo_set_source_rgb(cairo, color.r, color.g, color.b);
	cairo_rectangle(
	  cairo,
	  pos.x * path_width,
	  pos.y * path_width,
	  path_width,
	  path_width
	);
	cairo_fill(cairo);

	/* true if the direction is open for movement */
	bool north = false; /* negative y */
	bool south = false;
	bool east = false; /* negative x */
	bool west = false;
	bool up __attribute__((unused)) = false;
	bool down __attribute__((unused)) = false;

	cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
	for (Agedge_t *e = agfstedge(maze, node); NULL != e; e = agnxtedge(maze, e, node)) {
	  Agnode_t *other = e->node;
	  struct maze_pt3 other_pos;
	  if (0 == maze_read_location(maze, other, &other_pos)) {
		if (other_pos.y < pos.y) {
		  north = true;
		}
		if (other_pos.y > pos.y) {
		  south = true;
		}
		if (other_pos.x < pos.x) {
		  east = true;
		}
		if (other_pos.x > pos.x) {
		  west = true;
		}
		if (other_pos.z < pos.z) {
		  down = true;
		}
		if (other_pos.z > pos.z) {
		  up = true;
		}
	  }
	}

	if (do_walls) {
	  draw_walls(cairo, pos, path_width, north, south, east, west);
	}

	if (do_paths) {
	  draw_paths(cairo, pos, path_width, north, south, east, west);
	}
  }

  cairo_surface_write_to_png_stream(surface, write_to_stream, stream);
  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
}

int main(int argc, char** argv) {
  char *command_name = argv[0];

  struct option options[] = {
	{.name = "paths", .has_arg = optional_argument, .flag = NULL, .val = 'p'},
    {.name = "walls", .has_arg = optional_argument, .flag = NULL, .val = 'w'},
    {.name = "path-width", .has_arg = required_argument, .flag = NULL, .val = 'A' },
    {.name = NULL, .has_arg = 0, .flag = NULL, .val = 0}
  };

  bool do_paths = false;
  bool do_walls = true;
  uint path_width = DEFAULT_PATH_WIDTH;

  int opt;
  while (-1 != (opt = getopt_long(argc, argv, "", options, NULL))) {
	switch (opt) {
	case 'p':
	  if (NULL == optarg) {
		do_paths = true;
	  } else if (0 == strcmp("true", optarg)) {
		do_paths = true;
	  } else if (0 == strcmp("false", optarg)) {
		do_paths = false;
	  } else {
		fprintf(stderr, "argument to --paths must be \"true\" or \"false\"\n");
		usage(command_name);
		return 1;
	  }
	  break;
	case 'w':
	  if (NULL == optarg) {
		do_walls = true;
	  } else if (0 == strcmp("true", optarg)) {
		do_walls = true;
	  } else if (0 == strcmp("false", optarg)) {
		do_walls = false;
	  } else {
		fprintf(stderr, "argument to --walls must be \"true\" or \"false\"\n");
		usage(command_name);
		return 1;
	  }
	  break;
    case 'A':
      path_width = strtol(optarg, NULL, 10);
      break;
	default:
	  fprintf(stderr, "unrecognized option '%c'\n", opt);
	  usage(command_name);
	  return 1;
	}
  }

  if (0 == path_width) {
    fprintf(stderr, "path_width must be greater than zero\n");
    usage(command_name);
    return 1;
  }

  if (MAX_PATH_WIDTH < path_width) {
    fprintf(stderr, "path_width must be less that %d\n", MAX_PATH_WIDTH);
    usage(command_name);
    return 1;
  }

  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }

  struct maze_grid grid = maze_read_grid(maze);
  png_grid(maze, grid, stdout, path_width, do_walls, do_paths);
  maze_destroy_grid(grid);
  agclose(maze);
}
