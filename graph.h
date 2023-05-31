#pragma once
//we wish to profile performance, and we know the topology of our graph, so we will use a 2d array instead of a slow data structure
typedef unsigned char Cell;
typedef Cell* Graph;

Graph create_graph(int width, int height);

typedef struct coord {
    int x, y;
} Coord;

typedef struct path {
    Coord* steps;
    int num_steps;
    int nodes_pushed; //how many times we pushed onto the heap
    int largest_heap; //the largest that the heap got, we expect this will be close to nodes_pushed but want to confirm
} Path;

void free_path(Path* path);