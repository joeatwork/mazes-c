#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include <graphviz/cgraph.h>

struct maze_pt3 {
  unsigned long x;
  unsigned long y;
  unsigned long z;
};

struct maze_grid {
  struct maze_pt3 size;
  size_t nodes_count;
  Agnode_t **nodes;
};

int maze_read_location(Agraph_t *maze, Agnode_t *node, struct maze_pt3 *position);
struct maze_grid maze_read_grid(Agraph_t *maze);
Agnode_t *maze_find_in_grid_at_pt(Agraph_t *maze, struct maze_grid grid, struct maze_pt3 target);
void maze_destroy_grid(struct maze_grid grid);
#endif
