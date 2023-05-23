#pragma once

//we wish to profile performance, and we know the topology of our graph, so we will use a 2d array instead of a slow data structure

int* init_graph(int width, int height) {
    int* graph = malloc(sizeof(int) * width * height);
    if (graph == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(0);
    }
    return graph;
}