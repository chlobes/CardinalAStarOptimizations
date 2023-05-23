#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "graph.h"
#include "noise.h"
#include "astar.h"

int main() {
    int width, height, max_value;
    size_t seed;
    float scale;

    /*printf("Enter width: ");
    scanf("%d", &width);
    printf("Enter height: ");
    scanf("%d", &height);
    printf("Enter max value: ");
    scanf("%d", &max_value);
    printf("Enter seed: ");
    scanf("%zu", &seed);
    printf("Enter scale: ");
    scanf("%f", &scale);*/
    width = 4;
    height = 4;
    max_value = 5;
    seed = 3;
    scale = 1.0f;

    int* graph = init_graph(width, height);
    fill_with_noise(width, height, graph, max_value, seed, scale);

    FILE* fp = fopen("output.txt", "w");
    if (fp == NULL) {
        fprintf(stderr, "Could not open output file\n");
        free(graph);
        return 1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            fprintf(fp, "%d ", graph[y * width + x]);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "\n");

    Path path = astar(graph, width, height);

    printf("total path steps %d\n", path.num_steps);
    printf("total path cost %d\n", path.cost);

    // Create a temporary grid for printing.
    int* tempGrid = malloc(width * height * sizeof(int));
    memcpy(tempGrid, graph, width * height * sizeof(int));

    // Replace path nodes in the temporary grid with a special marker, e.g., -1.
    for (int i = 0; i < path.num_steps; i++) {
        Coord c = path.steps[i];
        tempGrid[c.y * width + c.x] = -1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // If the node is on the path, print an "X", otherwise print the node cost.
            if (tempGrid[y * width + x] == -1) {
                fprintf(fp, "X ");
            }
            else {
                fprintf(fp, "%d ", tempGrid[y * width + x]);
            }
        }
        fprintf(fp, "\n");
    }

    free(tempGrid);

    fclose(fp);
    free(graph);
    return 0;
}