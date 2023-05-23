#include <stdio.h>
#include "heap.h"

typedef struct coord {
    int x, y;
} Coord;

typedef struct path {
    Coord* steps;
    int num_steps;
    int cost;
} Path;

Path backtrace_path(unsigned char* closedList, int width, int height, int num_steps, int cost, Coord end) {
    Path path;
    path.steps = (Coord*)malloc(num_steps * sizeof(Coord));
    path.num_steps = num_steps;
    path.cost = cost;

    Coord current = end;
    for (int step = num_steps - 1; step >= 0; step--) {
        path.steps[step] = current;

        int direction = closedList[current.y * width + current.x];
        current.x -= ((int)(direction >> 4)) - 1;
        current.y -= ((int)(direction & 0xF)) - 2;
    }

    return path;
}

//written with chatgpt's assistance, but it's really not great at something this complex

//this function constructs a path from any node in the left to any node on the right, diagonals are permitted
//note, the heuristic assumes there are no 0 nodes
Path astar(int* grid, int width, int height) {
    Heap openList = create_heap(width * 4);

    unsigned char* closedList = malloc(width * height * sizeof(unsigned char)); //packed direction of the previous node
    for (int i = 0; i < width * height; i++) {
        closedList[i] = 0;
    }

    // Add the starting nodes to the open list
    for (int y = 0; y < height; y++) {
        closedList[width * y + 0] = 1; //fill the closed list so we don't try to path to these nodes
        Node node;
        node.x = 0;
        node.y = y;
        node.g = grid[y * width + 0];
        node.h = width;
        node.f = width + grid[y * width + 0];
        node.steps = 1;
        heap_push(&openList, node);
    }

    while (openList.size > 0) {
        Node current = heap_pop(&openList);
        printf("checking node at (%d, %d)\n",current.x,current.y);

        if (current.x == width - 1) { //path found
            Coord end = { current.x, current.y };
            Path result = backtrace_path(closedList, width, height, current.steps, current.g, end);
            free_heap(&openList);
            free(closedList);
            return result;
        }

        for (int dx = -1; dx <= 1; dx++) { //iterate through neighbours
            for (int dy = -1; dy <= 1; dy++) {
                Node node;
                node.x = current.x + dx;
                node.y = current.y + dy;

                if (node.x < 0 || node.x >= width || node.y < 0 || node.y >= height) { //skip nodes outside the map
                    continue;
                }

                if (closedList[node.y * width + node.x] > 0) { //skip nodes we already have a path to, this is ok since our h is admissible
                    continue;
                }

                node.g = current.g + grid[node.y * width + node.x];
                node.h = width - node.x;
                node.f = node.g + node.h;
                node.steps = current.steps + 1;

                closedList[node.y * width + node.x] = ((dx + 1) << 4) + (dy + 2);
                heap_push(&openList, node);
            }
        }
    }

    printf("Error: no path found"); //this should be impossible
    exit(1);
}