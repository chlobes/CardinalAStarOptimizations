#pragma once
#include "graph.h"
#include <stdlib.h>

Graph create_graph(int width, int height) {
    Graph graph = malloc(sizeof(Cell) * width * height);
    return graph;
}

void free_path(Path* path) {
    free(path->steps);
}