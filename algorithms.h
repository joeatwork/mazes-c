#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include "mazes.h"

void mazes_generate_tree(struct mazes_maze *maze);
void mazes_generate_aldous_broder(struct mazes_maze *maze);
void mazes_generate_backtracker(struct mazes_maze *maze);

#endif
