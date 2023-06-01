#pragma once
#include "graph.h"
#include "heap.h"

void add_child(int dx, int dy, int dir, Node* current, Node* next, Graph graph, Graph closed_set, Heap* open_set, Path* result, Pos end, int* next_found);

Path lookahead(Graph graph, Pos start, Pos end);