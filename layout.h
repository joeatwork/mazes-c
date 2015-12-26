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

// Grid will only contain those nodes with positions such that 
struct maze_grid maze_read_grid(Agraph_t *maze);
void maze_destroy_grid(struct maze_grid grid);
#endif
