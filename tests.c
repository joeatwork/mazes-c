#include <glib.h>
#include <stdio.h>
#include <string.h>
#include "mazes.h"

void test_grid_location() {
  {
	struct mazes_maze maze = {
	  .dimensions = 2,
	  .size = (size_t[2]){3,2}
	};

	size_t location[2];

	mazes_grid_location(&maze, 0, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){0,0}), sizeof(location));

	mazes_grid_location(&maze, 1, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,0}), sizeof(location));

	mazes_grid_location(&maze, 3, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){0,1}), sizeof(location));

	mazes_grid_location(&maze, 4, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,1}), sizeof(location));
  }

  {
	struct mazes_maze maze = {
	  .dimensions = 3,
	  .size = (size_t[3]){4,3,2}
	};
	
	size_t location[3];

	mazes_grid_location(&maze, 0, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){0,0,0}), sizeof(location));

	mazes_grid_location(&maze, 1, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,0,0}), sizeof(location));

	mazes_grid_location(&maze, 4, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){0,1,0}), sizeof(location));

	mazes_grid_location(&maze, 5, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,1,0}), sizeof(location));

	mazes_grid_location(&maze, 12, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){0,0,1}), sizeof(location));

	mazes_grid_location(&maze, 13, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,0,1}), sizeof(location));

	mazes_grid_location(&maze, 17, location);
	g_assert_cmpmem(location, sizeof(location), ((size_t[]){1,1,1}), sizeof(location));
  }
}

void test_grid_index() {
  {
	struct mazes_maze maze = {
	  .dimensions = 2,
	  .size = (size_t[2]){3,2}
	};
	
	size_t location[2];
	size_t ix;

	location[0] = 0;
	location[1] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 0);

	location[0] = 1;
	location[1] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 1);

	location[0] = 0;
	location[1] = 1;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 3);

	location[0] = 1;
	location[1] = 1;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 4);
  }

  {
	struct mazes_maze maze = {
	  .dimensions = 3,
	  .size = (size_t[3]){4,3,2}
	};
	
	size_t location[3];
	size_t ix;

	location[0] = 0;
	location[1] = 0;
	location[2] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 0);

	location[0] = 1;
	location[1] = 0;
	location[2] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 1);

	location[0] = 0;
	location[1] = 1;
	location[2] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 4);

	location[0] = 1;
	location[1] = 1;
	location[2] = 0;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 5);

	location[0] = 0;
	location[1] = 0;
	location[2] = 1;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 12);

	location[0] = 1;
	location[1] = 0;
	location[2] = 1;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 13);

	location[0] = 1;
	location[1] = 1;
	location[2] = 1;
	ix = mazes_grid_index(&maze, location);
	g_assert(ix == 17);
  }
}

void test_maze_rectangle() {
  {
	struct mazes_maze maze;
	size_t size[2] = {3,1};
	mazes_maze_init(&maze, size, 2);
	mazes_maze_arrange_as_rectangle(&maze);

	g_assert_cmpuint(maze.grid[0].neighbors_length, ==, 4);
	g_assert_cmpuint(maze.grid[1].neighbors_length, ==, 4);
	g_assert_cmpuint(maze.grid[2].neighbors_length, ==, 4);
	
	g_assert(maze.grid[0].neighbors[1] == &(maze.grid[1]));
	g_assert(maze.grid[1].neighbors[0] == &(maze.grid[0]));
	g_assert(maze.grid[1].neighbors[1] == &(maze.grid[2]));
	g_assert(maze.grid[2].neighbors[0] == &(maze.grid[1]));
	mazes_maze_destroy(&maze);
  }
}

int main(int argc, char** argv) {
  g_test_init(&argc, &argv, NULL);

  g_test_add_func("/maze/grid_location", test_grid_location);
  g_test_add_func("/maze/grid_index", test_grid_index);
  g_test_add_func("/maze/rectangle", test_maze_rectangle);

  return g_test_run();
}
