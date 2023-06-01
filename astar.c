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

    Pos p = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path->steps[step] = p;
        
        //reverse the step that was done previously
        Cell dir = cell(closed_set, p);
        int dx = (dir % 2) ? 0 : (dir - 3) * -1;
        int dy = (dir % 2) ? (dir - 4) * -1 : 0;
        p.x -= dx;
        p.y -= dy;
    }
}

//written with chatgpt's assistance, but it's really not great at something this complex

//regular old A* on a uniform grid, diagonals are not permitted
//any cell that isn't a zero is treated as a wall
//modifies the closed_set in the process of pathfinding so we can see how it works
Path astar(Graph closed_set, Pos start, Pos end) {
    Path result;
    Heap open_set = create_heap((closed_set.width * closed_set.height + 7) / 8);
    Node child, parent;

    int h = abs(start.x - end.x) + abs(start.y - end.y);
    child.pos = start;
    child.g = 1;
    child.h = heuristic(start, end);
    child.f = child.g + child.h;
    child.from = 2;
    heap_push(&open_set, child);
    result.nodes_discovered = 1;
    result.nodes_pushed = 1;
    result.nodes_expanded = 0;
    result.largest_heap = 1;

    while (open_set.size > 0) {
        parent = heap_pop(&open_set);

        if (cell(closed_set, parent.pos)) continue; //already checked
        set_cell(closed_set, parent.pos, parent.from);
        result.nodes_expanded++;

        if (parent.pos.x == end.x && parent.pos.y == end.y) { //found the destination
            backtrace_path(&result, closed_set, parent.g, end);
            free_heap(&open_set);
            return result;
        }

        for (Cell i = 2; i < 6; i++) {
            child.pos = parent.pos;
            switch (i) {
                case 2: //right
                    if (child.pos.x + 1 >= closed_set.width) continue;
                    child.pos.x += 1;
                    break;
                case 3: // down
                    if (child.pos.y + 1 >= closed_set.height) continue;
                    child.pos.y += 1;
                    break;
                case 4: // left
                    if (child.pos.x - 1 < 0) continue;
                    child.pos.x -= 1;
                    break;
                case 5: // up
                    if (child.pos.y - 1 < 0) continue;
                    child.pos.y -= 1;
                    break;
            }

            if (cell(closed_set, child.pos)) continue; //wall or already checked

            child.g = parent.g + 1; //uniform cost of 1
            child.h = heuristic(child.pos, end);
            child.f = child.g + child.h;
            child.from = i;

            heap_push(&open_set, child);
            result.nodes_pushed++;
            result.nodes_discovered++;
            result.largest_heap = max(result.largest_heap, open_set.size);


        }
    }

    printf("Error: no path found");
    exit(1);
}