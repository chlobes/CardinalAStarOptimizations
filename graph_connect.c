#pragma once
#include "graph_connect.h"
#include "graph.h"
#include "rng.h"

//search for a random point of desired type, return -1 if we can't find one
Pos find(Graph graph, Cell target, int start_x, int start_y) {
    int x = start_x;
    int y = start_y;

    while (cell(graph, (Pos) { x, y }) != target) {
        x += 1;
        if (x >= graph.width) {
            x = 0;
            y += 1;
            y %= graph.height;
        }
        if (x == start_x && y == start_y) {
            return (Pos) { -1, -1 }; //no targets left in the graph
        }
    }
    return (Pos) { x, y }; //found one
}

//starts at a point and fills all zeroes with twos to mark a region
void flood(Graph graph, int x, int y) {
    if (x < 0 || y < 0 || x >= graph.width || y >= graph.height) { //don't go off the edge of the graph
        return;
    }

    if (cell(graph, (Pos){ x, y })) { //if it's a wall or already filled, skip
        return;
    }

    set_cell(graph, (Pos) { x, y }, 2);
    flood(graph, x + 1, y);
    flood(graph, x - 1, y);
    flood(graph, x, y + 1);
    flood(graph, x, y - 1);
}

//checks if all empty nodes of the graph are connected and randomly adds connections until they are
void connect_graph(Graph graph, u64* rng) {
    Pos prev;
    Pos next = find(graph, 0, 0, 0);

    while (1) {
        flood(graph, next.x, next.y);
        prev = next;
        next = find(graph, 0, (prev.x + graph.width / 2) % graph.width, (prev.y + graph.height / 2) % graph.height);
        if (next.x == -1) break;
        
        while (cell(graph, prev)) { //take random steps towards a known empty space until we hit an empty space, this will always terminate
            set_cell(graph, prev, 2); //each time we take a step, destroy the wall on this tile, creating a pathway towards the next empty tile
            int step_type = gen_random(rng) % 8; //50% chance of taking a random step, 50% chance of taking a step towards the goal
            switch (step_type) {
                case 0: //right
                    if (prev.x + 1 >= graph.width) continue;
                    prev.x += 1;
                    break;
                case 1: //down
                    if (prev.y + 1 >= graph.height) continue;
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
    for (int i = 0; i < graph.width * graph.height; i++) {
        if (graph.cells[i] == 2) {
            graph.cells[i] = 0;
        }
    }
}