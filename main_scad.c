#include <graphviz/cgraph.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "color.h"
#include "layout.h"

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s\n", command_name);
  fprintf(stderr, "reads a dot grid on stdin and prints openscad on standard out\n");
  fprintf(stderr, "expects input to have _0, _1, _2 attributes as created by 'grid'\n");
}

static void openscad_grid(Agraph_t *maze, FILE *stream) {
  fputs("module edge(x, y, z) {\n", stdout);
  fputs("    length = norm([x, y, z]);\n", stdout);
  fputs("    inclination = acos(z/length);\n", stdout);
  fputs("    azimuth = atan2(y,x);\n", stdout);
  fputs("    rotate([0, inclination, azimuth])\n", stdout);
  fputs("        cylinder(h=length, r=1);\n", stdout);
  fputs("}\n", stdout);
  fputs("\n", stdout);
  
  double scale = 4.0;
  for (Agnode_t *node = agfstnode(maze); NULL != node; node = agnxtnode(maze, node)) {
	struct maze_pt3 nodep;
	if (0 == maze_read_location(maze, node, &nodep)) {
	  double source_x = scale * nodep.x;
	  double source_y = scale * nodep.y;
	  double source_z = scale * nodep.z;
	  struct maze_rgb color;
	  bool has_color = (0 == maze_read_rgb(node, &color));
	  if (has_color) {
		fprintf(stdout, "color([%f, %f, %f]) ", color.r, color.g, color.b);
	  }
	  fprintf(stdout, "translate([%f, %f, %f]) sphere(r=1); // %s\n",
			  source_x, source_y, source_z, agnameof(node));

	  for (Agedge_t *e = agfstedge(maze, node); NULL != e; e = agnxtedge(maze, e, node)) {
		Agnode_t *other = agtail(e);
		struct maze_pt3 otherp;
		if (other != node && 0 == maze_read_location(maze, other, &otherp)) {
		  double dest_x = scale * otherp.x;
		  double dest_y = scale * otherp.y;
		  double dest_z = scale * otherp.z;
		  double delta_x = dest_x - source_x;
		  double delta_y = dest_y - source_y;
		  double delta_z = dest_z - source_z;
		  if (has_color) {
			fprintf(stdout, "color([%f, %f, %f]) ", color.r, color.g, color.b);
		  }
		  fprintf(stdout, "translate([%f, %f, %f]) // %s -> %s\n",
				  source_x, source_y, source_z,
				  agnameof(node), agnameof(other));
		  fprintf(stdout, "    edge(%f, %f, %f);\n",
				  delta_x, delta_y, delta_z);
		}
	  }
	}
  }
}

int main(int argc, char** argv) {
  char *command_name = argv[0];
  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }
  
  openscad_grid(maze, stdout);
  agclose(maze);
}
