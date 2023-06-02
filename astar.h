#pragma once
#include "graph.h"

#define RIGHT 2
#define DOWN 3
#define LEFT 4
#define UP 5
#define DOWNRIGHT 6
#define DOWNLEFT 7
#define UPRIGHT 8
#define UPLEFT 9

Pos offset(Pos p, Cell d);

int heuristic(Pos pos, Pos end);

void backtrace_path(Path* path, Graph closed_set, int num_steps, Pos end);

Path astar(Graph closed_set, Pos start, Pos end);