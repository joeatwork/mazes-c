# A collection of tools for making and messing with mazes

## Briefly

    ./grid --width=10 --height=10 | ./maze --algorithm=backtracker | ./print

Mazes-c are a collections of tools that you can use to make and render
mazes. They're inspired by Jamis Buck's great book "Mazes for
Programmers" but is mostly tailored to make cool stuff that I like to
look at.

## The tools

* `grid` - produces a "maze" where all paths are open. You can use
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

* `maze` - takes the format produced by grid and produces a maze by
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

* `print` - takes a maze on stdin and renders it as ascii art to
  stdout.

* `png` - takes a maze on stdin and writes a png to stdout.

* `scad` - takes a maze on stdin and writes an openscad script to
  stdout.

* `color` - takes a maze on stdin and produces an annotated maze on
  stdout with added "color" attributes, that png or scad can use
  during rendering.

# Building the tools

Building mazes-c requires graphviz's cgraph headers and library, and
cairo. The tests require glib-2.0. I build on Ubuntu wily, with
libcairo2-dev and libgraphviz-dev, using gcc.

To build, I just run

    make

To run the (not really actual) tests, run

    make run-tests
