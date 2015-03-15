#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <stdio.h>
#include "mazes.h"

/* Assumes a rectangular maze*/
void mazes_fprint(struct mazes_maze *maze, FILE *stream);

#endif
