#include <assert.h>
#include <string.h>
#include "printer.h"
#include "utils.h"

/* RECTANGULAR mazes only */
void mazes_print(struct mazes_maze *maze, unsigned int *colors, FILE *stream) {
  /* For now, only renders the bottom floor of a maze */

  const size_t SOUTH_NEIGHBOR = 1;
  const size_t EAST_NEIGHBOR = 3;
  
  fputc('+', stream);
  for (size_t col = 0; col < maze->size[0]; col++) {
    fputs("--+", stream);
  }
  fputc('\n', stream);

  size_t *location = checked_calloc(maze->dimensions, sizeof(size_t));
  memset(location, 0, sizeof(size_t) * maze->dimensions);
  
  for (size_t row = 0; row < maze->size[1]; row++) {
    fputc('|', stream);
    for (size_t col = 0; col < maze->size[0]; col++) {
	  location[1] = row;
	  location[0] = col;
	  
      struct mazes_cell *cell = mazes_cell_at(maze, location);
      struct mazes_cell *eastern = cell->neighbors[EAST_NEIGHBOR];
      if (NULL == colors) {
        fputs("  ", stream);
      } else {
        fprintf(stream, "%2d", colors[cell->cell_number]);
      }

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
    for (size_t col = 0; col < maze->size[0]; col++) {
	  location[0] = col;
      struct mazes_cell *cell = mazes_cell_at(maze, location);
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

  free(location);
}
