#pragma once
#include "lookahead.h"
#include "astar.h" //we reuse backtrace_path
#include "heap.h"
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

//using a macro because passing in 20 arguments to a function is too verbose and hard to read
#define add_child(dir) do {\
    child.pos = offset(parent.pos, dir);\
    if (cell(closed_set, child.pos)) break;\
    child.g = parent.g + 1;\
    child.h = heuristic(child.pos, end);\
    child.f = child.g + child.h;\
    child.from = dir;\
    if (!next_found && child.h < parent.h) {\
        next = child;\
        next_found = 1;\
    } else {\
        heap_push(&open_set, child);\
    }\
} while (0)

//an optimization of A* that takes advantage of properties of the uniform grid
Path lookahead(Graph closed_set, Pos start, Pos end) {
    Path result;
    Heap open_set = create_heap(1);
    Node parent, child, next; //optimization 3: we can sometimes skip heap insertions using a next variable
    int next_found = 0;

    int h = abs(start.x - end.x) + abs(start.y - end.y);
    parent = (Node){ start.x, start.y, 1, h, h + 1, 1 };
    set_cell(closed_set, start, 1);
    #ifdef PATH_INFO
    result.nodes_discovered = 5;
    result.nodes_pushed = 0;
    result.nodes_expanded = 1;
    result.largest_heap = 0;
    #endif

    //we need to manually expand the first node if we want to optimize bounds checks in the main loop
    for (unsigned char dir = 2; dir < 6; dir++) {
        child.pos = offset(parent.pos, dir);
        
        if (child.pos.x < 0 || child.pos.y < 0 || child.pos.x >= closed_set.width || child.pos.y >= closed_set.height) continue; //out of bounds
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
        #ifdef PATH_INFO
        result.nodes_expanded++;
        #endif

        if (parent.pos.x == end.x && parent.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, parent.g, end);
            free_heap(&open_set);
            return result;
        }

        next_found = 0;

        #ifdef PATH_INFO
        result.nodes_discovered += 3;
        int before_size = open_set.size;
        #endif
        switch (parent.from) {
            case RIGHT:
                if (parent.pos.x + 1 < closed_set.width) {
                    add_child(RIGHT);
                }
                if (parent.pos.y + 1 < closed_set.height) {
                    if (cell(closed_set, offset(parent.pos, DOWNLEFT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(DOWN);
                    }
                }
                if (parent.pos.y - 1 >= 0) {
                    if (cell(closed_set, offset(parent.pos, UPLEFT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(UP);
                    }
                }
                break;
            case DOWN:
                if (parent.pos.y + 1 < closed_set.height) {
                    add_child(DOWN);
                }
                if (parent.pos.x + 1 < closed_set.width) {
                    if (cell(closed_set, offset(parent.pos, UPRIGHT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(RIGHT);
                    }
                }
                if (parent.pos.x - 1 >= 0) {
                    if (cell(closed_set, offset(parent.pos, UPLEFT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(LEFT);
                    }
                }
                break;
            case LEFT:
                if (parent.pos.x - 1 >= 0) {
                    add_child(LEFT);
                }
                if (parent.pos.y + 1 < closed_set.height) {
                    if (cell(closed_set, offset(parent.pos, DOWNRIGHT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(DOWN);
                    }
                }
                if (parent.pos.y - 1 >= 0) {
                    if (cell(closed_set, offset(parent.pos, UPRIGHT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(UP);
                    }
                }
                break;
            case UP:
                if (parent.pos.y - 1 >= 0) {
                    add_child(UP);
                }
                if (parent.pos.x - 1 >= 0) {
                    if (cell(closed_set, offset(parent.pos, DOWNLEFT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(LEFT);
                    }
                }
                if (parent.pos.x + 1 < closed_set.width) {
                    if (cell(closed_set, offset(parent.pos, DOWNRIGHT))) { //optimization 2: if grandparent could've pathed to this node, skip it
                        add_child(RIGHT);
                    }
                }
                break;
        }
        #ifdef PATH_INFO
        result.largest_heap = max(result.largest_heap, open_set.size);
        result.nodes_pushed += open_set.size - before_size;
        #endif

        if (!next_found) {
            if (open_set.size == 0) break;
            next = heap_pop(&open_set);
        }
    }

    printf("Error: no path found");
    exit(1);
}