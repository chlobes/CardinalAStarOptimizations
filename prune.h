#pragma once

#include "graph.h"
#include "rng.h"

//search for a random point of desired type, return INT_MAX if we can't find one
Coord find(Graph graph, int width, int height, Cell target, u64* rng) {
    int start_x = gen_random(rng) % width;
    int start_y = gen_random(rng) % height;
    int x = start_x;
    int y = start_y;

    while (graph[x + width * y] != target) {
        x += 1;
        if (x >= width) {
            x = 0;
            y += 1;
            y %= height;
        }
        if (x == start_x && y == start_y) {
            return (Coord) { INT_MAX, INT_MAX }; //no points left in the graph
        }
    }
    return (Coord) { x, y }; //found one
}

//starts at a point and fills all zeroes with twos to mark a region
void flood(Graph graph, int width, int height, int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) { //don't go off the edge of the graph
        return;
    }

    if (graph[x + width * y] != 0) { //if it's a wall or already filled, skip
        return;
    }

    graph[x + width * y] = 2;
    flood(graph, width, height, x + 1, y);
    flood(graph, width, height, x - 1, y);
    flood(graph, width, height, x, y + 1);
    flood(graph, width, height, x, y - 1);
}

//checks if all empty nodes of the graph are connected and randomly adds connections until they are 
void connect_graph(Graph graph, int width, int height, u64* rng) {
    Coord from, empty;
    empty = find(graph, width, height, 0, rng);
    flood(graph, width, height, empty.x, empty.y);
    empty = find(graph, width, height, 0, rng);

    while (empty.x != INT_MAX) {
        from = find(graph, width, height, 2, rng);
        int dx = from.x < empty.x ? 1 : -1;
        int dy = from.y < empty.y ? 1 : -1;
        while (from.x != empty.x && from.y != empty.y) { //take random orthogonal steps towards empty to create a pathway
            graph[from.x + width * from.y] = 2;
            if (gen_random(rng) % 2) {
                from.x += dx;
            }
            else {
                from.y += dy;
            }
        }
        while (from.x != empty.x) {
            graph[from.x + width * from.y] = 2;
            from.x += dx;
        }
        while (from.y != empty.y) {
            graph[from.x + width * from.y] = 2;
            from.y += dy;
        }
        flood(graph, width, height, empty.x, empty.y);
        empty = find(graph, width, height, 0, rng);
    }
    //we're finished, now set all the twos back to zeroes
    for (int i = 0; i < width * height; i++) {
        if (graph[i] == 2) {
            graph[i] = 0;
        }
    }
}