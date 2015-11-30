#include <stdlib.h>
#include <cairo/cairo.h>
#include "mazes.h"
#include "pnger.h"
#include "utils.h"

#define PATH_WIDTH_PIXELS 10

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
  /* width and height rely on low MAX_GRID_DIMENSION to avoid overflow */

  unsigned int max_color = 0;
  if (NULL != colors) {
    size_t max_cell_number = MAZE_SIZE(maze);
    for (int i = 0; i < max_cell_number; i++) {
      if (colors[i] > max_color) {
        max_color = colors[i];
      }
    }
  }

  size_t width = maze->column_count * PATH_WIDTH_PIXELS;
  size_t height = maze->row_count * PATH_WIDTH_PIXELS;
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB16_565, width + 1, height + 1);
  cairo_t *cairo = cairo_create(surface);
  cairo_rectangle(cairo, 0.0, 0.0, width + 1, height + 1);
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_fill(cairo);

  cairo_set_line_width(cairo, 1.0);
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    size_t row = cell->row;
    size_t col = cell->column;
    struct mazes_cell *northern = cell->neighbors[NORTH_NEIGHBOR];
    struct mazes_cell *southern = cell->neighbors[SOUTH_NEIGHBOR];
    struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
    struct mazes_cell *western = cell->neighbors[WEST_NEIGHBOR];

    if (max_color > 0) {
      double intensity = 1.0 - (((double) colors[cell->cell_number]) / max_color);
      cairo_set_source_rgb(cairo, 0.0, intensity, 0.0);
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
