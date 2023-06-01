#pragma once
#include <stdlib.h>
#include "graph.h"

//binary heap implementation for astar's priority queue, credit to chatgpt
//except that it created a max heap when I wanted a min heap because it's silly

typedef struct node {
    Pos pos;
    int g;
    int h;
    int f;
    Cell from;
} Node;

typedef struct heap {
    Node* data;
    int capacity;
    int size;
} Heap;

Heap create_heap(int capacity);

void heap_push(Heap* heap, Node node);

Node heap_pop(Heap* heap);

void free_heap(Heap* heap);