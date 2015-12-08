#include "dotter.h"

void mazes_dot(struct mazes_maze *maze, unsigned int *colors, FILE *stream) {
  fprintf(stream, "/** MAZE: Rectangle ");
  for (size_t i = 0; i < maze->dimensions; i++) {
    fprintf(stream, "%zd ", maze->size[i]);
  }
  fprintf(stream, "**/\n");

  fprintf(stream, "digraph Maze {\n");
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    fprintf(stream, "    node_%zd ", cell->cell_number);
    fprintf(stream, "[shape=box,label=\"");
    for (size_t i = 0; i < maze->dimensions; i++) {
      if (i > 0) {
        fprintf(stream, ",");
      }
      fprintf(stream, "%zd", cell->location[i]);
    }
    fprintf(stream, "\",");

    for (size_t i = 0; i < maze->dimensions; i++) {
      if (i > 0) {
        fprintf(stream, ",");
      }
      fprintf(stream, "_%zd=%zd", i, cell->location[i]);
    }
    fprintf(stream, "];\n");
    for (size_t i = 0; i < cell->links_length; i++) {
      struct mazes_cell *other = cell->links[i];
      fprintf(stream, "    node_%zd -> node_%zd;\n",
              cell->cell_number, other->cell_number);
    }
  }
  fprintf(stream, "}\n");
}
