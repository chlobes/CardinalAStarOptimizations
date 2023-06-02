#pragma once
#include "astar.h"
#include "heap.h"
#include "graph.h"
#include "stdio.h"
#include <stdlib.h>
#include <stdio.h>

Pos offset(Pos p, Cell d) {
    switch (d) {
        case RIGHT: p.x += 1; break;
        case DOWN: p.y += 1; break;
        case LEFT: p.x -= 1; break;
        case UP: p.y -= 1; break;
        case DOWNRIGHT: p.x += 1, p.y += 1; break;
        case DOWNLEFT: p.x -= 1, p.y += 1; break;
        case UPRIGHT: p.x += 1, p.y -= 1; break;
        case UPLEFT: p.x -= 1, p.y -= 1; break;
    }
    return p;
}

int heuristic(Pos pos, Pos end) { //manhattan distance is admissible in this case
    return abs(pos.x - end.x) + abs(pos.y - end.y);
}

void backtrace_path(Path* path, Graph closed_set, int num_steps, Pos end) {
    path->steps = (Pos*)malloc(num_steps * sizeof(Pos));
    path->num_steps = num_steps;

    Pos p = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path->steps[step] = p;
        
        //reverse the step that was done previously
        Cell dir = cell(closed_set, p);
        switch (dir) {
            case RIGHT: dir = LEFT; break;
            case DOWN: dir = UP; break;
            case LEFT: dir = RIGHT; break;
            case UP: dir = DOWN; break;
        }
        p = offset(p, dir);
    }
}

//written with chatgpt's assistance, but it's really not great at something this complex

//regular old A* on a uniform grid, diagonals are not permitted
//any cell that isn't a zero is treated as a wall
//modifies the closed_set in the process of pathfinding so we can see how it works
Path astar(Graph closed_set, Pos start, Pos end) {
    Path result;
    Heap open_set = create_heap(1);
    Node child, parent;

    parent.pos = start;
    parent.g = 1;
    parent.h = heuristic(start, end);
    parent.f = parent.g + parent.h;
    parent.from = RIGHT;
    heap_push(&open_set, parent);
    #ifdef PATH_INFO
    result.nodes_discovered = 1;
    result.nodes_pushed = 1;
    result.nodes_expanded = 0;
    result.largest_heap = 1;
    #endif

    while (open_set.size > 0) {
        parent = heap_pop(&open_set);

        if (cell(closed_set, parent.pos)) continue; //already checked
        set_cell(closed_set, parent.pos, parent.from);
        #ifdef PATH_INFO
        result.nodes_expanded++;
        #endif

        if (parent.pos.x == end.x && parent.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, parent.g, end);
            free_heap(&open_set);
            return result;
        }

        for (Cell dir = 2; dir < 6; dir++) {
            #ifdef PATH_INFO
            result.nodes_discovered++;
            #endif
            child.pos = offset(parent.pos, dir);

            if (child.pos.x < 0 || child.pos.y < 0 || child.pos.x >= closed_set.width || child.pos.y >= closed_set.height) continue; //out of bounds

            if (cell(closed_set, child.pos)) continue; //wall or already checked

            child.g = parent.g + 1; //uniform cost of 1
            child.h = heuristic(child.pos, end);
            child.f = child.g + child.h;
            child.from = dir;

            heap_push(&open_set, child);
            #ifdef PATH_INFO
            result.nodes_pushed++;
            result.largest_heap = max(result.largest_heap, open_set.size);
            #endif
        }
    }

    printf("Error: no path found");
    exit(1);
}