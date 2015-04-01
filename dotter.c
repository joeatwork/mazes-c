#include "dotter.h"

void mazes_dot(struct mazes_maze *maze, unsigned int *colors, FILE *stream) {
  fprintf(stream, "/** MAZE: Rectangle %d %d **/\n", maze->column_count, maze->row_count);
  fprintf(stream, "digraph Maze {\n");
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    for (size_t i = 0; i < cell->links_length; i++) {
      struct mazes_cell *other = cell->links[i];
      fprintf(stream, "    node_%d_%d -> node_%d_%d;\n",
	      cell->column, cell->row, other->column, other->row);
    }
  }
  fprintf(stream, "}\n");
}
