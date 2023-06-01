#pragma once
#include "graph.h"

#define max(a, b) (a > b ? a : b)

int heuristic(Pos pos, Pos end);

void backtrace_path(Path* path, Graph closed_set, int num_steps, Pos end);

Path astar(Graph graph, Pos start, Pos end);