#pragma once
#include "graph.h"
#include <stdlib.h>

Graph create_graph(int width, int height) {
    Graph r;
    r.width = width;
    r.height = height;
    r.cells = malloc(width * height, sizeof(Cell));
    return r;
}

Cell cell(Graph graph, Pos pos) {
    return graph.cells[pos.x + graph.width * pos.y];
}

void set_cell(Graph graph, Pos pos, Cell val) {
    graph.cells[pos.x + graph.width * pos.y] = val;
}

void free_graph(Graph graph) {
    free(graph.cells);
}

void free_path(Path* path) {
    free(path->steps);
}