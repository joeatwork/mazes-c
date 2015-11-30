#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "distances.h"
#include "utils.h"

void mazes_distances(
  unsigned int *distances,
  struct mazes_maze *maze,
  struct mazes_cell *start
) {
  size_t maze_size = MAZE_SIZE(maze);
  for (size_t i = 0; i < maze_size; i++) {
    distances[i] = 0;
  }
  
  for (struct mazes_cell *cell = mazes_first_cell(maze); NULL != cell; cell = cell->next) {
    cell->mark = NULL;
  }

  struct mazes_cell **stack = checked_calloc(maze_size, sizeof(struct mazes_cell *));
  size_t stack_depth = 0;

  stack[stack_depth] = start;
  stack_depth++;

  while (stack_depth > 0) {
    assert(stack_depth < maze_size);
    struct mazes_cell *home = stack[stack_depth - 1];
    stack_depth--;

    unsigned int home_distance = distances[home->cell_number];

    home->mark = home;
    for (size_t i = 0; i < home->links_length; i++) {
      struct mazes_cell *next = home->links[i];
      if (NULL == next->mark) {
        distances[next->cell_number] = home_distance + 1;
        stack[stack_depth] = next;
        stack_depth++;
      }
    }
  }
}
