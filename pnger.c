#include <stdlib.h>
#include <cairo/cairo.h>
#include "mazes.h"
#include "pnger.h"

#define PATH_WIDTH_PIXELS 10

/* Assumes BIDIRECTIONAL and rectangular Mazes! */
void mazes_png(struct mazes_maze *maze, const char *filename) {
  /* width and height rely on low MAX_GRID_DIMENSION to avoid overflow */
  size_t width = maze->column_count * PATH_WIDTH_PIXELS;
  size_t height = maze->row_count * PATH_WIDTH_PIXELS;
  cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB16_565, width + 1, height + 1);
  cairo_t *cairo = cairo_create(surface);
  cairo_rectangle(cairo, 0.0, 0.0, width + 1, height + 1);
  cairo_set_source_rgb(cairo, 1.0, 1.0, 1.0);
  cairo_fill(cairo);

  cairo_set_line_width(cairo, 1.0);
  cairo_set_source_rgb(cairo, 0.0, 0.0, 0.0);
  for (size_t row = 0; row < maze->row_count; row++) {
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *cell = mazes_cell_at(maze, row, col);
      struct mazes_cell *northern = cell->neighbors[NORTH_NEIGHBOR];
      struct mazes_cell *southern = cell->neighbors[SOUTH_NEIGHBOR];
      struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
      struct mazes_cell *western = cell->neighbors[WEST_NEIGHBOR];

      if (NULL == northern) {
	cairo_move_to(cairo, col * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
	cairo_line_to(cairo, (col + 1) * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
	cairo_stroke(cairo);
      }

      if (NULL == western) {
	cairo_move_to(cairo, col * PATH_WIDTH_PIXELS, row * PATH_WIDTH_PIXELS);
	cairo_line_to(cairo, col * PATH_WIDTH_PIXELS, (row + 1) * PATH_WIDTH_PIXELS)
;	cairo_stroke(cairo);
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
  }
  
  cairo_surface_write_to_png(surface, filename);
  cairo_destroy(cairo);
  cairo_surface_destroy(surface);
}
