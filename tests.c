#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "layout.h"
#include "utils.h"

void test_runs() {
  g_assert(1 == 1);
}

void test_find_empty_grid() {
  Agraph_t *maze = agopen("maze", Agstrictundirected, NULL);
  struct maze_grid grid = maze_read_grid(maze);
  g_assert(0 == grid.nodes_count);

  struct maze_pt3 target = {
	.x = 0,
	.y = 0,
	.z = 0
  };

  Agnode_t * found = maze_find_in_grid_at_pt(maze, grid, target);
  g_assert(NULL == found);

  target.x = 1;
  target.y = 2;

  found = maze_find_in_grid_at_pt(maze, grid, target);
  g_assert(NULL == found);

  maze_destroy_grid(grid);
  agclose(maze);
}

void test_find_higher_grid() {
  Agraph_t *maze = agopen("maze", Agstrictundirected, NULL);
  Agnode_t *node = agnode(maze, "high", true);
  agsafeset(node, "_0", "3", "");
  agsafeset(node, "_1", "3", "");
  agsafeset(node, "_2", "3", "");

  struct maze_grid grid = maze_read_grid(maze);
  g_assert(1 == grid.nodes_count);

  struct maze_pt3 target = {
	.x = 0,
	.y = 0,
	.z = 0
  };

  Agnode_t * found = maze_find_in_grid_at_pt(maze, grid, target);
  g_assert(node == found);

  target.x = 1;
  target.y = 2;

  found = maze_find_in_grid_at_pt(maze, grid, target);
  g_assert(node == found);

  target.x = 10;
  target.y = 10;
  target.z = 10;

  found = maze_find_in_grid_at_pt(maze, grid, target);
  g_assert(NULL == found);

  maze_destroy_grid(grid);
  agclose(maze);
}


void test_grid_order() {
  FILE *maze_in = popen("./grid --width=4 --height=4", "r");
  if (NULL == maze_in) {
	ERROR_EXIT("Can't load test fixture");
  }

  Agraph_t *maze = agread(maze_in, NULL);
  pclose(maze_in);

  struct maze_grid grid = maze_read_grid(maze);
  g_assert(16 == grid.nodes_count);

  for (size_t i = 0; i < grid.nodes_count; i++) {
	struct maze_pt3 target = {
	  .x = i % 4,
	  .y = i / 4,
	  .z = 0
	};
	Agnode_t * found = maze_find_in_grid_at_pt(maze, grid, target);
	g_assert(found == grid.nodes[i]);
  }

  maze_destroy_grid(grid);
  agclose(maze);
}

int main(int argc, char** argv) {
  g_test_init(&argc, &argv, NULL);
  g_test_add_func("/maze/test_runs", test_runs);
  g_test_add_func("/maze/grid_order", test_grid_order);
  g_test_add_func("/maze/find_empty_grid", test_find_empty_grid);
  g_test_add_func("/maze/find_higher_grid", test_find_higher_grid);
  return g_test_run();
}
