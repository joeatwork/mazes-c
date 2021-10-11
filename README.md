# A collection of tools for making and messing with mazes

## Briefly

    ./grid --width=10 --height=10 | ./maze --algorithm=backtracker | ./print

Mazes-c are a collections of tools that you can use to make and render
mazes. They're inspired by Jamis Buck's great book "Mazes for
Programmers" but are tailored to making stuff that I like to
look at.

## The tools

- `grid` - produces a "maze" where all paths are open. You can use
  grid as the first step in a pipeline that makes mazes. The resulting
  maze is printed to stdout in a format that is similar to, and
  vaguely compatible with, the graphviz dot format. You shouldn't have
  to interact with this format directly - to see the grid, use the
  `print` tool.

```
$ ./grid --width=20 --height=5 | ./print

+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|                                                           |
+  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +
|                                                           |
+  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +
|                                                           |
+  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +
|                                                           |
+  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +  +
|                                                           |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```

- `maze` - takes the format produced by grid and produces a maze by
  removing edges. Reads from stdin and writes to stdout.

```
$ ./grid --width=20 --height=5 | ./maze | ./print

+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
|     |           |        |        |        |              |
+  +  +  +  +--+--+  +  +--+  +  +  +  +  +--+  +--+--+--+  +
|  |  |  |           |        |  |  |  |     |     |        |
+  +--+  +--+--+  +--+--+--+--+  +  +  +--+  +--+  +  +--+  +
|        |     |           |  |  |  |     |  |     |     |  |
+  +--+--+--+  +--+--+--+  +  +  +--+--+  +  +  +--+--+  +  +
|  |        |  |              |           |     |     |  |  |
+  +  +--+  +  +--+--+--+--+--+--+--+--+--+--+--+  +  +  +  +
|     |     |                                      |     |  |
+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
```

- `print` - takes a maze on stdin and renders it as ascii art to
  stdout.

- `png` - takes a maze on stdin and writes a png to stdout.

- `scad` - takes a maze on stdin and writes an openscad script to
  stdout.

There are some other little maze toys included with the software as well.
Refer to the usage messages of the programs for more detailed help.

- `assemble` translates a list of three dimensional coordinates into
  a format that ./maze and friends can use, allowing you to create shaped
  mazes.

- `color` - takes a maze on stdin and produces an annotated maze on
  stdout with added "color" attributes, that png or scad can use
  during rendering.

- `gravity` takes a maze as input and attempts to add edges so it will
  print more easily on a 3D printer.

- `paths` reads a maze in the common, dot-ish format, and produces
  JSON that can be used to reconstruct the maze.

You can also run a (toy, insecure) server using `server.sh`, and run
that server in a Docker image using the included `Dockerfile`.

# Building the tools

Building mazes-c requires graphviz's cgraph headers and library, and
cairo. The tests require glib-2.0. I build on Ubuntu, with
libcairo2-dev and libgraphviz-dev, using gcc.

To build, I just run

    make

To run the (not really actual) tests, run

    make run-tests

A more detailed description of the requirements to build the system
can be inferred from the Dockerfile included with this project. You
can build the dockerfile (which includes the very silly, certainly
insecure maze service) you can run.

    make container
