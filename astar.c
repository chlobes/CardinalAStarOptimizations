#pragma once
#include "astar.h"
#include "heap.h"
#include "graph.h"
#include "stdio.h"
#include <stdlib.h>

int heuristic(Pos pos, Pos end) { //manhattan distance is admissible in this case
    return abs(pos.x - end.x) + abs(pos.y - end.y);
}

void backtrace_path(Path* path, Graph closed_set, int num_steps, Pos end) {
    path->steps = (Pos*)malloc(num_steps * sizeof(Pos));
    path->num_steps = num_steps;

    Pos current = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path->steps[step] = current;
        
        //reverse the step that was done previously
        switch (cell(closed_set, current)) {
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
Path astar(Graph graph, Pos start, Pos end) {
    Path result;
    Heap open_set = create_heap((graph.width * graph.height + 7) / 8);
    
    Graph closed_set = create_graph(graph.width, graph.height);

    int h = abs(start.x - end.x) + abs(start.y - end.y);
    Node node;
    node.pos = start;
    node.g = 1;
    node.h = heuristic(start, end);
    node.f = node.g + node.h;
    node.from = 1;
    heap_push(&open_set, node);
    result.nodes_pushed = 1;
    result.largest_heap = 1;

    while (open_set.size > 0) {
        Node current = heap_pop(&open_set);

        if (cell(closed_set, current.pos)) continue; //already checked
        set_cell(closed_set, current.pos, current.from);

        if (current.pos.x == end.x && current.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, current.g, end);
            free_heap(&open_set);
            free_graph(closed_set);
            return result;
        }

        Node node;
        for (Cell i = 1; i < 5; i++) {
            node.pos = current.pos;
            switch (i) {
                case 1: //right
                    if (node.pos.x + 1 >= graph.width) continue;
                    node.pos.x += 1;
                    break;
                case 2: // down
                    if (node.pos.y + 1 >= graph.height) continue;
                    node.pos.y += 1;
                    break;
                case 3: // left
                    if (node.pos.x - 1 < 0) continue;
                    node.pos.x -= 1;
                    break;
                case 4: // up
                    if (node.pos.y - 1 < 0) continue;
                    node.pos.y -= 1;
                    break;
            }

            if (cell(graph, node.pos) || cell(closed_set, node.pos)) continue; //wall or already checked

            node.g = current.g + 1; //uniform cost of 1
            node.h = heuristic(node.pos, end);
            node.f = node.g + node.h;
            node.from = i;

            heap_push(&open_set, node);
            result.nodes_pushed += 1;
            result.largest_heap = max(result.largest_heap, open_set.size);


        }
    }

    printf("Error: no path found");
    exit(1);
}