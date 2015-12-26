#include <graphviz/cgraph.h>
#include <cairo/cairo.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "color.h"
#include "layout.h"
#include "utils.h"

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s\n", command_name);
  fprintf(stderr, "reads a dot grid on stdin and prints an ascii maze on stdout\n");
  fprintf(stderr, "expects input to have _0, _1, _2 attributes as created by 'grid'\n");
}

static cairo_status_t write_to_stream(void *closure, const unsigned char *data, unsigned int length) {
  FILE *stream = closure;
  fwrite(data, sizeof(unsigned char), length, stream);
  if (ferror(stream)) {
    ERROR_EXIT("Error writing to output file");
  }
  return CAIRO_STATUS_SUCCESS;
}

#define PATH_WIDTH_PIXELS 10

static void png_grid(Agraph_t *maze, struct maze_grid grid, FILE *stream) {
  if (grid.size.x > (INT_MAX / (PATH_WIDTH_PIXELS + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  if (grid.size.y > (INT_MAX / (PATH_WIDTH_PIXELS + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  if (grid.size.z > (INT_MAX / (PATH_WIDTH_PIXELS + 1)) - 1) {
	ERROR_EXIT("maze is too large to render into a png");
  }

  int width = grid.size.x * PATH_WIDTH_PIXELS;
  int height = grid.size.y * PATH_WIDTH_PIXELS;

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
	  pos.x * PATH_WIDTH_PIXELS,
	  pos.y * PATH_WIDTH_PIXELS,
	  PATH_WIDTH_PIXELS,
	  PATH_WIDTH_PIXELS
	);
	cairo_fill(cairo);

	bool north = false;
	bool south = false;
	bool east = false;
	bool west = false;
	bool up __attribute__((unused)) = false; 
	bool down __attribute__((unused)) = false;

	cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
	for (Agedge_t *e = agfstedge(maze, node); NULL != e; e = agnxtedge(maze, e, node)) {
	  Agnode_t *other = e->node;
	  struct maze_pt3 other_pos;
	  if (0 == maze_read_location(maze, other, &other_pos)) {
		if (other_pos.y > pos.y) {
		  south = true;
		}
		if (other_pos.y < pos.y) {
		  north = true;
		}		
		if (other_pos.x > pos.x) {
		  east = true;
		}
		if (other_pos.x < pos.x) {
		  west = true;
		}
		if (other_pos.z > pos.z) {
		  up = true;
		}
		if (other_pos.z < pos.z) {
		  down = true;
		}
	  }
	}

	if (!north) {
	  cairo_move_to(cairo, pos.x * PATH_WIDTH_PIXELS, pos.y * PATH_WIDTH_PIXELS);
	  cairo_line_to(cairo, (pos.x + 1) * PATH_WIDTH_PIXELS, pos.y * PATH_WIDTH_PIXELS);
	  cairo_stroke(cairo);
	}

	if (!south) {
	  cairo_move_to(cairo, pos.x * PATH_WIDTH_PIXELS, (pos.y + 1) * PATH_WIDTH_PIXELS);
	  cairo_line_to(cairo, (pos.x + 1) * PATH_WIDTH_PIXELS, (pos.y + 1) * PATH_WIDTH_PIXELS);
	  cairo_stroke(cairo);
	}
	
	if (!west) {
	  cairo_move_to(cairo, pos.x * PATH_WIDTH_PIXELS, pos.y * PATH_WIDTH_PIXELS);
	  cairo_line_to(cairo, pos.x * PATH_WIDTH_PIXELS, (pos.y + 1) * PATH_WIDTH_PIXELS);
	  cairo_stroke(cairo);
	}

	if (!east) {
	  cairo_move_to(cairo, (pos.x + 1) * PATH_WIDTH_PIXELS, pos.y * PATH_WIDTH_PIXELS);
	  cairo_line_to(cairo, (pos.x + 1) * PATH_WIDTH_PIXELS, (pos.y + 1) * PATH_WIDTH_PIXELS);
	  cairo_stroke(cairo);
	}
  }

  cairo_surface_write_to_png_stream(surface, write_to_stream, stream);
  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
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
  png_grid(maze, grid, stdout);
  maze_destroy_grid(grid);
  agclose(maze);
}
