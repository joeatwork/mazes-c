#include <stdio.h>
#include "mazes.h"
#include "printer.h"

int main(int argc, char** argv) {
  struct mazes_maze maze;
  mazes_maze_init(&maze, 0, 0);
  mazes_fprint(&maze, stdout);
  mazes_maze_destroy(&maze);

  mazes_maze_init(&maze, 1, 0);
  mazes_fprint(&maze, stdout);
  mazes_maze_destroy(&maze);

  mazes_maze_init(&maze, 0, 1);
  mazes_fprint(&maze, stdout);
  mazes_maze_destroy(&maze);

  mazes_maze_init(&maze, 5, 5);
  mazes_fprint(&maze, stdout);
  mazes_maze_destroy(&maze);
}
