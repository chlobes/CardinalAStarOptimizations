#pragma once
#include "lookahead.h"
#include "astar.h" //we reuse backtrace_path
#include "heap.h"
#include "graph.h"
#include <stdlib.h>

void add_child(int dx, int dy, int dir, Node* current, Node* next, Graph graph, Graph closed_set, Heap* open_set, Path* result, Pos end, int* next_found) {
    Node child;
    child.pos.x = current->pos.x + dx;
    child.pos.y = current->pos.y + dy;
    child.g = current->g + 1;
    child.h = heuristic(child.pos, end);
    child.f = child.g + child.h;

    if (cell(graph, child.pos) || cell(closed_set, child.pos)) return;
    set_cell(closed_set, child.pos, dir);
    if (!(*next_found) && child.h < current->h) {
        *next = child;
        *next_found = 1;
    }
    else {
        heap_push(open_set, child);
        result->nodes_pushed += 1;
        result->largest_heap = max(result->largest_heap, open_set->size);
    }
}

//an optimization of A* that takes advantage of properties of the uniform grid
Path lookahead(Graph graph, Pos start, Pos end) {
    Path result;
    Heap open_set = create_heap((graph.width * graph.height + 7) / 8);
    Node current, next; //optimization 3: we can sometimes skip heap insertions using a next variable
    int next_found = 0;

    Graph closed_set = create_graph(graph.width, graph.height);

    int h = abs(start.x - end.x) + abs(start.y - end.y);
    current = (Node){ start.x, start.y, 1, h, h + 1, 1 };
    set_cell(closed_set, start, 1);
    result.nodes_pushed = 0;
    result.largest_heap = 0;

    //we need to manually expand the first node if we want to optimize bounds checks in the main loop
    for (unsigned char i = 1; i < 5; i++) {
        int dx = (i % 2) ? (i - 2) * -1 : 0;
        int dy = (i % 2) ? 0 : (i - 3) * -1;
        
        if (current.pos.x + dx < 0 || current.pos.y + dy < 0 || current.pos.x + dx >= graph.width || current.pos.y + dy >= graph.height) continue; //out of bounds

        add_child(dx, dy, i, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
    }

    while (1) {
        current = next;

        if (current.pos.x == end.x && current.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, current.g, end);
            free_heap(&open_set);
            free_graph(closed_set);
            return result;
        }

        next_found = 0;

        switch (cell(closed_set, current.pos)) { //optimization 1: check where we moved from so we don't check the parent
        case 1: //right
            if (current.pos.x + 1 < graph.width) {
                add_child(1, 0, 1, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
            }
            if (current.pos.y + 1 < graph.height) {
                if (cell(graph, (Pos) { current.pos.x - 1, current.pos.y + 1 })) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(0, 1, 2, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                }
            }
            if (current.pos.y - 1 >= 0) {
                //if (graph[current.x - 1 + width * (current.y - 1)]) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(0, -1, 4, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                //}
            }
            break;
        case 2: //down
            if (current.pos.y + 1 < graph.height) {
                add_child(0, 1, 2, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
            }
            if (current.pos.x + 1 < graph.width) {
                //if (graph[current.x + 1 + width * (current.y - 1)]) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(1, 0, 1, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                //}
            }
            if (current.pos.x - 1 >= 0) {
                if (cell(graph, (Pos) { current.pos.x - 1, current.pos.y - 1 })) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(-1, 0, 3, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                }
            }
            
            break;
        case 3: //left
            if (current.pos.x - 1 >= 0) {
                add_child(-1, 0, 3, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
            }
            if (current.pos.y + 1 < graph.height) {
                //if (graph[current.x + 1 + width * (current.y + 1)]) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(0, 1, 2, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                //}
            }
            if (current.pos.y - 1 >= 0) {
                if (cell(graph, (Pos) { current.pos.x + 1, current.pos.y - 1 })) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(0, -1, 4, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                }
            }
            break;
        case 4: //up
            if (current.pos.y - 1 >= 0) {
                add_child(0, -1, 4, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
            }
            if (current.pos.x - 1 >= 0) {
                //if (graph[current.x - 1 + width * (current.y + 1)]) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(-1, 0, 3, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                //}
            }
            if (current.pos.x + 1 < graph.width) {
                if (cell(graph, (Pos) { current.pos.x + 1, current.pos.y + 1 })) { //optimization 2: if our parent could've pathed to this node, skip it
                    add_child(1, 0, 1, &current, &next, graph, closed_set, &open_set, &result, end, &next_found);
                }
            }
            break;
        }

        if (!next_found) {
            if (open_set.size == 0) break;
            next = heap_pop(&open_set);
        }
    }

    printf("Error: no path found");
    exit(1);
}