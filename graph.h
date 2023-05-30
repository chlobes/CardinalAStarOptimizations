#pragma once

typedef int Cell;
typedef Cell* Graph;

//we wish to profile performance, and we know the topology of our graph, so we will use a 2d array instead of a slow data structure

Graph create_graph(int width, int height) {
    Graph graph = malloc(sizeof(Cell) * width * height);
    if (graph == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(0);
    }
    return graph;
}

typedef struct coord {
    int x, y;
} Coord;

typedef struct path {
    Coord* steps;
    int num_steps;
    //int cost;
} Path;

void free_path(Path* path) {
    free(path->steps);
}