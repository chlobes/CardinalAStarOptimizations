#pragma once
#include "graph_connect.h"
#include "graph.h"
#include "bmp.h"
#include <stdlib.h>

//floods zeroes with twos
void flood_empty(Graph graph, Pos* stack, Pos p) {
    int stack_size = 0;

    set_cell(graph, p, 2);
    stack[stack_size++] = p;

    while (stack_size) {
        p = stack[--stack_size];
        for (int dir = 0; dir < 4; dir++) {
            int dx = (dir % 2) ? 0 : (dir - 1);
            int dy = (dir % 2) ? (dir - 2) : 0;
            Pos u = (Pos){ p.x + dx, p.y + dy };
            if (u.x < 0 || u.y < 0 || u.x >= graph.width || u.y >= graph.height) continue;
            if (!cell(graph, u)) {
                set_cell(graph, u, 2);
                stack[stack_size++] = u;
            }
        }
    }
}

//finds the nearest zero, returns (0, 0) if it can't find one
Pos find_empty(Graph graph, Pos* stack, Pos p) {
    int head = 0;
    int tail = 0;

    if (cell(graph, p) == 1) set_cell(graph, p, 3);

    stack[head++] = p;

    while(head > tail) {
        p = stack[tail++];
        for (int dir = 0; dir < 4; dir++) {
            int dx = (dir % 2) ? 0 : (dir - 1);
            int dy = (dir % 2) ? (dir - 2) : 0;
            Pos u = (Pos){ p.x + dx, p.y + dy };
            if (u.x < 0 || u.y < 0 || u.x >= graph.width || u.y >= graph.height) continue;
            switch (cell(graph, u)) {
                case 0:
                    for (int i = 0; i < head; i++) { //clean up markings
                        if (cell(graph, stack[i]) == 3) set_cell(graph, stack[i], 1);
                        if (cell(graph, stack[i]) == 4) set_cell(graph, stack[i], 2);
                    }
                    return u;
                case 1:
                    set_cell(graph, u, 3); //mark it as visited so we don't keep flood filling the same cells
                    stack[head++] = u;
                    break;
                case 2:
                    set_cell(graph, u, 4);
                    stack[head++] = u;
            }
        }
    }
    for (int i = 0; i < head; i++) { //clean up markings
        if (cell(graph, stack[i]) == 3) set_cell(graph, stack[i], 1);
        if (cell(graph, stack[i]) == 4) set_cell(graph, stack[i], 2);
    }
    return (Pos) { 0, 0 };
}

//destroys walls until all empty tiles are contiguous
void connect_graph(Graph graph) {
    Pos* stack = malloc(graph.width * graph.height * sizeof(Pos));

    Pos p = (Pos){ 0, 0 };
    Pos next = p;

    do {
        int dx = (p.x < next.x) ? 1 : -1; //smash walls as we move to the empty spot to ensure it'll be connected to our space
        while (p.x != next.x && cell(graph, p)) {
            set_cell(graph, p, 2);
            p.x += dx;
        }
        int dy = (p.y < next.y) ? 1 : -1;
        while (p.y != next.y && cell(graph, p)) {
            set_cell(graph, p, 2);
            p.y += dy;
        } //now p == next, so we can repeat
        flood_empty(graph, stack, p); //mark all the empty spots we can reach as reachable
        next = find_empty(graph, stack, p); //find the closest empty spot
    } while (next.x != 0 || next.y != 0);

    for (int i = 0; i < graph.width * graph.height; i++) { //unmark all the cells we operated on
        if (graph.cells[i] > 1) graph.cells[i] -= 2;
    }

    free(stack);
}