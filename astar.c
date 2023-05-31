#pragma once
#include "astar.h"
#include "heap.h"
#include "graph.h"
#include <stdlib.h>

void backtrace_path(Path* path, unsigned char* closed_set, int width, int height, int num_steps, Coord end) {
    path->steps = (Coord*)malloc(num_steps * sizeof(Coord));
    path->num_steps = num_steps;

    Coord current = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path->steps[step] = current;
        
        //reverse the step that was done previously
        switch (closed_set[current.y * width + current.x]) {
            case 1: //right
                current.x -= 1;
                break;
            case 2: // down
                current.y -= 1;
                break;
            case 3: // left
                current.x += 1;
                break;
            case 4: // up
                current.y += 1;
                break;
        }
    }
}

//written with chatgpt's assistance, but it's really not great at something this complex

//regular old A* on a uniform grid, diagonals are not permitted
//any cell that isn't a zero is treated as a wall
Path astar(Graph graph, int width, int height, Coord start, Coord end) {
    Path result;
    Heap open_set = create_heap((width * height + 7) / 8);

    unsigned char* closed_set = malloc(width * height * sizeof(unsigned char));
    memset(closed_set, 0, width * height * sizeof(unsigned char));

    closed_set[start.x + width * start.y] = 1;
    Node node = (Node) { start.x, start.y, 1, 0, 1 };
    heap_push(&open_set, node);
    result.nodes_pushed = 1;
    result.largest_heap = 1;

    while (open_set.size > 0) {
        Node current = heap_pop(&open_set);



        Node node;
        for (unsigned char i = 1; i < 5; i++) {
            switch (i) {
                case 1: //right
                    if (current.x + 1 >= width) continue;
                    node.x = current.x + 1;
                    node.y = current.y;
                    break;
                case 2: // down
                    if (current.y + 1 >= height) continue;
                    node.x = current.x;
                    node.y = current.y + 1;
                    break;
                case 3: // left
                    if (current.x - 1 < 0) continue;
                    node.x = current.x - 1;
                    node.y = current.y;
                    break;
                case 4: // up
                    if (current.y - 1 < 0) continue;
                    node.x = current.x;
                    node.y = current.y - 1;
                    break;
            }

            if (graph[node.x + width * node.y] || closed_set[node.x + width * node.y]) continue; //wall or already checked

            node.g = current.g + 1; //uniform cost of 1
            node.h = abs(node.x - end.x) + abs(node.y - end.y); //heuristic is manhattan distance, this should be admissible
            node.f = node.g + node.h;

            closed_set[node.x + width * node.y] = i;
            heap_push(&open_set, node);
            result.nodes_pushed += 1;
            result.largest_heap = max(result.largest_heap, open_set.size);

            if (node.x == end.x && node.y == end.y) { //found the destination
                backtrace_path(&result, closed_set, width, height, node.g, end);
                free_heap(&open_set);
                free(closed_set);
                return result;
            }
        }
    }

    printf("Error: no path found");
    exit(1);
}