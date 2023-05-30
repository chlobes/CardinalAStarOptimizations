#include "heap.h"
#include "graph.h"
#include <stdlib.h>

#define dist(a, b) (abs((a) - (b)))

Path backtrace_path(unsigned char* closed_set, int width, int height, int num_steps, Coord end) {
    Path path;
    path.steps = (Coord*)malloc(num_steps * sizeof(Coord));
    path.num_steps = num_steps;
    //path.cost = cost;

    Coord current = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path.steps[step] = current;

        unsigned char direction = closed_set[current.y * width + current.x];
        current.x -= ((int)(direction >> 4)) - 1;
        current.y -= ((int)(direction & 0xF)) - 2;
    }

    return path;
}

//written with chatgpt's assistance, but it's really not great at something this complex

//regular old A* on a uniform grid, diagonals are not permitted
//any cell that isn't a zero is treated as a wall
Path astar(Graph graph, int width, int height, Coord start, Coord end) {
    Heap open_set = create_heap((width * height + 7) / 8);

    unsigned char* closed_set = malloc(width * height * sizeof(unsigned char));
    memset(closed_set, 0, width * height * sizeof(unsigned char));

    closed_set[start.x + width * start.y] = 1;
    Node node = (Node) { start.x, start.y, 1, 0, 1 };

    while (open_set.size > 0) {
        Node current = heap_pop(&open_set);

        if (current.x == end.x && current.y == end.y) { //found the destination
            Path result = backtrace_path(closed_set, width, height, current.g, end);
            free_heap(&open_set);
            free(closed_set);
            return result;
        }

        for (int i = 0; i < 4; i++) {
            int dir = (i / 2) * -2 + 1;
            int dx = (i % 2) * dir;
            int dy = dir - dx;

            Node node;
            node.x = current.x + dx;
            node.y = current.y + dy;

            if (node.x < 0 || node.x >= width || node.y < 0 || node.y >= height) { //out of bounds
                continue;
            }

            if (graph[node.y * width + node.x] || closed_set[node.y * width + node.x]) { //wall or already checked
                continue;
            }

            node.g = current.g + 1; //uniform cost of 1
            node.h = abs(node.x - end.x) + abs(node.y - end.y); //heuristic is manhattan distance, this should be admissible
            node.f = node.g + node.h;

            closed_set[node.x + width * node.y] = ((dx + 1) << 4) + (dy + 2);
            heap_push(&open_set, node);
        }
    }

    printf("Error: no path found");
    exit(1);
}