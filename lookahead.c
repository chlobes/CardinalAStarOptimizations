#pragma once
#include "lookahead.h"
#include "astar.h" //we reuse backtrace_path
#include "heap.h"
#include "graph.h"
#include <stdlib.h>

#define add_child(dir) do {\
    int dx = (dir % 2) ? 0 : (dir - 3) * -1;\
    int dy = (dir % 2) ? (dir - 4) * -1 : 0;\
    child.pos.x = parent.pos.x + dx;\
    child.pos.y = parent.pos.y + dy;\
    if (cell(closed_set, child.pos)) break;\
    child.g = parent.g + 1;\
    child.h = heuristic(child.pos, end);\
    child.f = child.g + child.h;\
    child.from = dir;\
    if (!next_found && child.h < parent.h) {\
        next = child;\
        next_found = 1;\
    }\
    else {\
        heap_push(&open_set, child);\
        result.nodes_pushed += 1;\
        result.largest_heap = max(result.largest_heap, open_set.size);\
    }\
} while (0)

//an optimization of A* that takes advantage of properties of the uniform grid
Path lookahead(Graph graph, Pos start, Pos end) {
    Path result;
    Graph closed_set = create_graph(graph.width, graph.height);
    Heap open_set = create_heap((closed_set.width * closed_set.height + 7) / 8);
    Node parent, child, next; //optimization 3: we can sometimes skip heap insertions using a next variable
    int next_found = 0;

    memcpy(closed_set.cells, graph.cells, closed_set.width * closed_set.height * sizeof(Cell)); //optimization: merge closed_set with graph

    int h = abs(start.x - end.x) + abs(start.y - end.y);
    parent = (Node){ start.x, start.y, 1, h, h + 1, 1 };
    set_cell(closed_set, start, 1);
    result.nodes_pushed = 0;
    result.largest_heap = 0;

    //we need to manually expand the first node if we want to optimize bounds checks in the main loop
    for (unsigned char dir = 2; dir < 6; dir++) {
        int dx = (dir % 2) ? 0 : (dir - 3) * -1;
        int dy = (dir % 2) ? (dir - 4) * -1 : 0;
        
        if (parent.pos.x + dx < 0 || parent.pos.y + dy < 0 || parent.pos.x + dx >= closed_set.width
            || parent.pos.y + dy >= closed_set.height) continue; //out of bounds

        add_child(dir);
    }

    while (1) {
        parent = next;

        if (cell(closed_set, parent.pos)) { //already checked
            if (open_set.size == 0) break;
            next = heap_pop(&open_set);
            continue;
        }
        set_cell(closed_set, parent.pos, parent.from);

        if (parent.pos.x == end.x && parent.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, parent.g, end);
            free_heap(&open_set);
            free_graph(closed_set);
            return result;
        }

        next_found = 0;

        switch (parent.from) { //optimization 1: check where we moved from so we don't check the parent
        case 2: //right
            if (parent.pos.x + 1 < closed_set.width) {
                add_child(2);
            }
            if (parent.pos.y + 1 < closed_set.height) {
                if (cell(graph, (Pos) { parent.pos.x - 1, parent.pos.y + 1 })) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(3);
                }
            }
            if (parent.pos.y - 1 >= 0) {
                //if (graph[parent.x - 1 + width * (parent.y - 1)]) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(5);
                //}
            }
            break;
        case 3: //down
            if (parent.pos.y + 1 < closed_set.height) {
                add_child(3);
            }
            if (parent.pos.x + 1 < closed_set.width) {
                //if (graph[parent.x + 1 + width * (parent.y - 1)]) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(2);
                //}
            }
            if (parent.pos.x - 1 >= 0) {
                if (cell(graph, (Pos) { parent.pos.x - 1, parent.pos.y - 1 })) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(4);
                }
            }
            
            break;
        case 4: //left
            if (parent.pos.x - 1 >= 0) {
                add_child(4);
            }
            if (parent.pos.y + 1 < closed_set.height) {
                //if (graph[parent.x + 1 + width * (parent.y + 1)]) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(3);
                //}
            }
            if (parent.pos.y - 1 >= 0) {
                if (cell(graph, (Pos) { parent.pos.x + 1, parent.pos.y - 1 })) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(5);
                }
            }
            break;
        case 5: //up
            if (parent.pos.y - 1 >= 0) {
                add_child(5);
            }
            if (parent.pos.x - 1 >= 0) {
                //if (graph[parent.x - 1 + width * (parent.y + 1)]) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(4);
                //}
            }
            if (parent.pos.x + 1 < closed_set.width) {
                if (cell(graph, (Pos) { parent.pos.x + 1, parent.pos.y + 1 })) { //optimization 2: if grandparent could've pathed to this node, skip it
                    add_child(2);
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