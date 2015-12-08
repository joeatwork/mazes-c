#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <cairo/cairo.h>
#include "mazes.h"
#include "pnger.h"
#include "utils.h"

#define PATH_WIDTH_PIXELS 10

// Cargo-culted from a stack-overflow answer
// (in particular, http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both)
struct rgb {
  double r;
  double g;
  double b;
};

static struct rgb hsv2rgb(double hh) {
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    int i = (long)hh;
    double ff = hh - i;
    double p = 0;
    double q = 1.0 - ff;

    struct rgb out;
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

static cairo_status_t write_to_stream(void *closure, const unsigned char *data, unsigned int length) {
  FILE *stream = closure;
  fwrite(data, sizeof(unsigned char), length, stream);
  if (ferror(stream)) {
    ERROR_EXIT("Error writing to output file");
  }
  return CAIRO_STATUS_SUCCESS;
}

/* Assumes BIDIRECTIONAL and rectangular Mazes! */
void mazes_png(struct mazes_maze *maze, unsigned int *colors, FILE *stream) {
  assert(2 == maze->dimensions);

  for (size_t i = 0; i < maze->dimensions; i++) {
    if (maze->size[i] > (INT_MAX / (PATH_WIDTH_PIXELS + 1)) - 1) {
      ERROR_EXIT("maze is too large to render into a png\n");
    }
  }

  unsigned int max_color = 0;
  if (NULL != colors) {
    size_t max_cell_number = mazes_maze_size(maze);
    for (int i = 0; i < max_cell_number; i++) {
      if (colors[i] > max_color) {
        max_color = colors[i];
      }
    }
  }

  size_t width = maze->size[0] * PATH_WIDTH_PIXELS;
  size_t height = maze->size[1] * PATH_WIDTH_PIXELS;
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB16_565, width + 1, height + 1);
  cairo_t *cairo = cairo_create(surface);
  cairo_rectangle(cairo, 0.0, 0.0, width + 1, height + 1);
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_fill(cairo);

  cairo_set_line_width(cairo, 1.0);

  struct rgb increments = {0,0,0};
  double start_hue = 360.0 * ((double) rand()/RAND_MAX);
  double end_hue = fmod(start_hue + 180.0, 360.0);
  struct rgb start_color = hsv2rgb(start_hue);
  struct rgb end_color = hsv2rgb(end_hue);

  const size_t NORTH_NEIGHBOR = 0;
  const size_t SOUTH_NEIGHBOR = 1;
  const size_t WEST_NEIGHBOR = 2;
  const size_t EAST_NEIGHBOR = 3;
  
  if (max_color > 0) {
    increments.r = (end_color.r - start_color.r) / max_color;
    increments.g = (end_color.g - start_color.g) / max_color;
    increments.b = (end_color.b - start_color.b) / max_color;
  }

  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    size_t row = cell->location[1];
    size_t col = cell->location[0];	
    struct mazes_cell *northern = cell->neighbors[NORTH_NEIGHBOR];
    struct mazes_cell *southern = cell->neighbors[SOUTH_NEIGHBOR];
    struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
    struct mazes_cell *western = cell->neighbors[WEST_NEIGHBOR];

    if (max_color > 0) {
      double r = start_color.r + increments.r * colors[cell->cell_number];
      double g = start_color.g + increments.g * colors[cell->cell_number];
      double b = start_color.b + increments.b * colors[cell->cell_number];

      cairo_set_source_rgb(cairo, r, g, b);
      cairo_rectangle(
          cairo,
          col * PATH_WIDTH_PIXELS,
          row * PATH_WIDTH_PIXELS,
          PATH_WIDTH_PIXELS,
          PATH_WIDTH_PIXELS
      );
      cairo_fill(cairo);
    }

    cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
    if (NULL == northern) {
      cairo_move_to(cairo, col * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
      cairo_line_to(cairo, (col + 1) * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
      cairo_stroke(cairo);
    }

    if (NULL == western) {
      cairo_move_to(cairo, col * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
      cairo_line_to(cairo, col * PATH_WIDTH_PIXELS, (row + 1) * PATH_WIDTH_PIXELS);
      cairo_stroke(cairo);
    }

    if (!mazes_cells_are_linked(cell, eastern)) {
      cairo_move_to(cairo, (col + 1) * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
      cairo_line_to(cairo, (col + 1) * PATH_WIDTH_PIXELS, (row + 1) * PATH_WIDTH_PIXELS);
      cairo_stroke(cairo);
    }

    if (!mazes_cells_are_linked(cell, southern)) {
      cairo_move_to(cairo, col * PATH_WIDTH_PIXELS, (row + 1) * PATH_WIDTH_PIXELS);
      cairo_line_to(cairo, (col + 1) * PATH_WIDTH_PIXELS, (row + 1) * PATH_WIDTH_PIXELS);
      cairo_stroke(cairo);
    }
  }

  cairo_surface_write_to_png_stream(surface, write_to_stream, stream);
  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
}
