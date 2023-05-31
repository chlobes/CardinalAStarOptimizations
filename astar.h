#pragma once
#include "graph.h"

#define max(a, b) (a > b ? a : b)

void backtrace_path(Path* path, unsigned char* closed_set, int width, int height, int num_steps, Coord end);

Path astar(Graph graph, int width, int height, Coord start, Coord end);