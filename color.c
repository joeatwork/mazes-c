#include <stdlib.h>
#include "color.h"

int maze_read_rgb(Agnode_t *node, struct maze_rgb *color) {
  char *val;
  if (NULL != (val = agget(node, "color"))) {
	char *end;
	double red = strtod(val, &end);
	if (',' != *end) goto fail;
	double green = strtod(end + 1, &end);
	if (',' != *end) goto fail;
	double blue = strtod(end + 1, &end);
	if ('\0' != *end) goto fail;

	color->r = red;
	color->g = green;
	color->b = blue;

	return 0;
  }

 fail:
  return -1;
}
