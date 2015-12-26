#ifndef __COLOR_H__
#define __COLOR_H__

#include <graphviz/cgraph.h>

struct maze_rgb {
  double r;
  double g;
  double b;
};

int maze_read_rgb(Agnode_t *node, struct maze_rgb *color);

#endif
