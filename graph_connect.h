#pragma once

#include "graph.h"
#include "rng.h"

//search for a random point of desired type, return INT_MAX if we can't find one
Coord find(Graph graph, int width, int height, Cell target, int start_x, int start_y) {
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
            return (Coord) { INT_MAX, INT_MAX }; //no targets left in the graph
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
    Coord prev;
    Coord next = find(graph, width, height, 0, 0, 0);

    while (1) {
        flood(graph, width, height, next.x, next.y);
        prev = next;
        next = find(graph, width, height, 0, (prev.x + width / 2) % width, (prev.y + height / 2) % height);
        if (next.x == INT_MAX) break;
        
        while (graph[prev.x + width * prev.y]) { //take random steps towards a known empty space until we hit an empty space, this will always terminate
            graph[prev.x + width * prev.y] = 2; //each time we take a step, destroy the wall on this tile, creating a pathway towards the next empty tile
            int step_type = gen_random(rng) % 8; //50% chance of taking a random step, 50% chance of taking a step towards the goal
            switch (step_type) {
                case 0: //right
                    if (prev.x + 1 >= width) continue;
                    prev.x += 1;
                    break;
                case 1: //down
                    if (prev.y + 1 >= height) continue;
                    prev.y += 1;
                    break;
                case 2: //left
                    if (prev.x - 1 < 0) continue;
                    prev.x -= 1;
                    break;
                case 3: //up
                    if (prev.y - 1 < 0) continue;
                    prev.y -= 1;
                    break;
                default: //towards goal
                    if (step_type % 2) { //towards goal x
                        if (prev.x == next.x) continue;
                        if (prev.x < next.x) {
                            prev.x += 1;
                        } else {
                            prev.x -= 1;
                        }
                    }
                    else { //towards goal y
                        if (prev.y == next.y) continue;
                        if (prev.y < next.y) {
                            prev.y += 1;
                        } else {
                            prev.y -= 1;
                        }
                    }
            }
        }
        next = prev; //set next to whatever empty tile we found first
    }
    //we're finished, now set all the twos back to zeroes
    for (int i = 0; i < width * height; i++) {
        if (graph[i] == 2) {
            graph[i] = 0;
        }
    }
}