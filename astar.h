#pragma once
#include "graph.h"

int heuristic(Pos pos, Pos end);

void backtrace_path(Path* path, Graph closed_set, int num_steps, Pos end);

Path astar(Graph closed_set, Pos start, Pos end);