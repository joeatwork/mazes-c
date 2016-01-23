#include <graphviz/cgraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "layout.h"
#include "utils.h"

static void usage(char *command_name) {
  fprintf(stderr, "usage: %s", command_name);
  fprintf(stderr, "reads a dot-formatted maze from stdin and produces a list of\n");
  fprintf(stderr, "non-overlapping paths on stdout. Output is a newline separated\n");
  fprintf(stderr, "list of JSON arrays, containing JSON objects with the attributes\n");
  fprintf(stderr, "and name of each node.\n");
}

static void dumpNode(Agraph_t *maze, Agnode_t *node) {
  Agsym_t *sym = NULL;
  bool printed = false;

  fprintf(stdout, "{\"name\":\"%s\",", agnameof(node));
  fprintf(stdout, "\"attributes\":{");
  while (NULL != (sym = agnxtattr(maze,AGNODE,sym))) {
	char *val = agxget(node, sym);
	if (NULL != val && val != sym->defval) {
	  if (printed) {
		fputc(',', stdout);
	  } else {
		printed = true;
	  }
	  fprintf(stdout, "\"%s\":\"%s\"", sym->name, val);
	}
  }
  fprintf(stdout, "}}");
}

int main(int argc, char** argv) {
  char *command_name = argv[0];
  Agraph_t *maze = agread(stdin, NULL);
  if (NULL == maze) {
	fprintf(stderr, "could not read maze from stdin\n");
	usage(command_name);
	return 1;
  }

  size_t stack_max = agnnodes(maze);
  if (stack_max == 0) {
	return 0;
  }

  size_t stack_depth = 0;
  Agnode_t **stack = checked_calloc(stack_max, sizeof(Agnode_t *));

  stack[stack_depth] = agfstnode(maze);
  agbindrec(stack[stack_depth], "mark", sizeof(Agrec_t), false);
  stack_depth++;

  while (0 < stack_depth) {
	Agnode_t *root = stack[stack_depth - 1];
	stack_depth--;

	fprintf(stdout, "[");
	dumpNode(maze, root);

	do {
	  Agnode_t *next = NULL;
	  bool has_unvisited = false;

	  // TODO BUG - what if there are no edges at all?
	  // TODO (shouldn't that just set root to NULL and be ok?)
	  for (Agedge_t *e = agfstedge(maze, root);
		   NULL != e && !has_unvisited;
		   e = agnxtedge(maze, e, root)) {
		Agnode_t *other = e->node;
		if (NULL == aggetrec(other, "mark", 0)) {
		  if (NULL == next) {
			next = other;
			agbindrec(next, "mark", sizeof(Agrec_t), false);
		  } else if (0 == stack_depth || stack[stack_depth - 1] != root) {
			stack[stack_depth] = root;
			stack_depth++;
			has_unvisited = true;
		  }
		}
	  }

	  root = next;
	  if (NULL != root) {
		fputc(',', stdout);
		dumpNode(maze, root);
	  }
	} while(root != NULL);

	fprintf(stdout, "]\n");
  }
  free(stack);
  agclose(maze);
}
