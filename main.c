#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define USER_INPUT 0
#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "graph.h"
#include "noise.h"
#include "astar.h"
#include "graph_connect.h"
#include "rng.h"
//#include "mincut.h"

//generates a graph of walls and empty nodes, the empty nodes are always connected, and the top left and bottom right right corners are always empty
Graph generate_graph(FILE* fp, int width, int height, u64 seed, float noise_thresh, float noise_scale) {
    Graph graph = create_graph(width, height);
    u64* rng = init_rng(seed);

    fill_with_noise(graph, width, height, rng, noise_thresh, noise_scale);
    graph[0] = 0;
    graph[width * height - 1] = 0;

    if (DEBUG) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                fprintf(fp, "%d ", graph[y * width + x]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    }

    connect_graph(graph, width, height, rng);

    return graph;
}

int main() {
    int width, height;
    u64 seed;
    float noise_thresh, noise_scale;

    if (USER_INPUT) {
        printf("enter grid width: ");
        scanf("%d", &width);
        printf("enter grid height: ");
        scanf("%d", &height);
        printf("enter rng seed: ");
        scanf("%lu", &seed);
        printf("enter noise threshold (-1 to 1): ");
        scanf("%f", &noise_thresh);
        printf("enter noise scale: ");
        scanf("%f", &noise_scale);
    } else {
        width = 10;
        height = 10;
        seed = 3;
        noise_thresh = -0.25f;
        noise_scale = 1.0f;
    }
    
    FILE* fp = fopen("output.txt", "w");

    Graph graph = generate_graph(fp, width, height, seed, noise_thresh, noise_scale);

    if (DEBUG) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                fprintf(fp, "%d ", graph[x + width * y]);
            }
            fprintf(fp, "\n");
        }
    }

    fprintf(fp, "\n");

    Path path = astar(graph, width, height, (Coord) { 0, 0 }, (Coord) { width - 1, height - 1 });

    printf("total path steps %d\n", path.num_steps);
    //printf("total path cost %d\n", path.cost);

    if (DEBUG) {
        // Create a temporary grid for printing.
        Graph tempGrid = create_graph(width, height);
        memcpy(tempGrid, graph, width * height * sizeof(Cell));

        // Replace path nodes in the temporary grid with a special marker -1
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
        fprintf(fp, "\n");
        free(tempGrid);
    }

    free_path(&path);

    //TODO: cellular automata comparison

    fclose(fp);
    free(graph);
    return 0;
}

