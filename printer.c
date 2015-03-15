#include <string.h>
#include "printer.h"
#include "utils.h"

/* RECTANGULAR mazes only. 0 width? 0 height?*/
void mazes_fprint(struct mazes_maze *maze, FILE *stream) {
  fputc('+', stream);
  for (size_t col = 0; col < maze->column_count; col++) {
    fputs("--+", stream);
  }
  fputc('\n', stream);

  for (size_t row = 0; row < maze->row_count; row++) {
    fputc('|', stream);
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *cell = mazes_cell_at(maze, row, col);
      struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
      fputs("  ", stream);
      if (NULL != eastern) {
	bool east_link = mazes_cells_are_linked(cell, eastern);
	bool west_link = mazes_cells_are_linked(eastern, cell);
	if (east_link && west_link) {
	  fputc(' ', stream);
	} else if (east_link) {
	  fputc('>', stream);
	} else if (west_link) {
	  fputc('<', stream);
	} else {
	  fputc('|', stream);
	}
      } else {
	fputc('|', stream);
      }
    }
    fputc('\n', stream);

    fputc('+', stream);
    for (size_t col = 0; col < maze->column_count; col++) {
      struct mazes_cell *cell = mazes_cell_at(maze, row, col);
      struct mazes_cell *southern = cell->neighbors[SOUTH_NEIGHBOR];
      if (NULL != southern) {
	bool south_link = mazes_cells_are_linked(cell, southern);
	bool north_link = mazes_cells_are_linked(southern, cell);
	if (south_link && north_link) {
	  fputs("  ", stream);
	} else if (south_link) {
	  fputs("!!", stream); /* "!" is the best character I could come up with? */
	} else if (north_link) {
	  fputs("^^", stream);
	} else {
	  fputs("--", stream);
	}
      } else {
	fputs("--", stream);
      }
      fputc('+', stream);
    }
    fputc('\n', stream);
  }
}
