#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "graph.h"
#include "noise.h"
#include "astar.h"
#include "graph_connect.h"
//#include "mincut.h"

int main() {
    int width, height;
    size_t seed;
    float threshhold, scale;

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
    width = 10;
    height = 10;
    threshhold = -0.3f;
    seed = 4;
    scale = 1.0f;

    Graph graph = create_graph(width, height);
    u64* rng = init_rng(seed);
    fill_with_noise(graph, width, height, threshhold, rng, scale);
    //line the graph with walls
    /*for (int x = 0; x < width; x++) {
        graph[x] = 1;
        graph[x + width * (height - 1)] = 1;
    }
    for (int y = 0; y < height; y++) {
        graph[width * y] = 1;
        graph[width - 1 + width * y] = 1;
    }*/
    //empty top left and bottom left corners because we want to path from corner to corner
    graph[0] = 0; 
    graph[width * height - 1] = 0;

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

    connect_graph(graph, width, height, rng);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            fprintf(fp, "%d ", graph[y * width + x]);
        }
        fprintf(fp, "\n");
    }
    return;

    fprintf(fp, "\n");

    Path path = astar(graph, width, height, (Coord) { 0, 0 }, (Coord) { width - 1, height - 1 });

    printf("total path steps %d\n", path.num_steps);
    //printf("total path cost %d\n", path.cost);

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

    free_path(&path);

    //TODO: cellular automata comparison

    free(tempGrid);

    fclose(fp);
    free(graph);
    return 0;
}

