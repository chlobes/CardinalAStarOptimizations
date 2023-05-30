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

typedef Path (*PathfinderFunction)(Graph, int, int, Coord, Coord);

//generates a graph of walls and empty nodes, the empty nodes are always connected, and the top left and bottom right right corners are always empty
Graph generate_graph(FILE* debug, int width, int height, u64 seed, float noise_thresh, float noise_scale) {
    Graph graph = create_graph(width, height);
    u64* rng = init_rng(seed);

    fill_with_noise(graph, width, height, rng, noise_thresh, noise_scale);
    graph[0] = 0;
    graph[width * height - 1] = 0;

    if (DEBUG) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                fprintf(debug, "%d ", graph[y * width + x]);
            }
            fprintf(debug, "\n");
        }
        fprintf(debug, "\n");
    }

    connect_graph(graph, width, height, rng);

    return graph;
}

//profiling function written by chatgpt
void profile_pathfinder(FILE* debug, FILE* output, PathfinderFunction f, Graph graph, int width, int height, Coord start, Coord end) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;
    double interval;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time); //start the timer

    Path path = f(graph, width, height, start, end);

    QueryPerformanceCounter(&end_time); //stop the timer
    interval = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    fprintf(output, "time %fs\n", interval);
    fprintf(output, "path length %d", path.num_steps);

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
                    fprintf(debug, "X ");
                }
                else {
                    fprintf(debug, "%d ", tempGrid[y * width + x]);
                }
            }
            fprintf(debug, "\n");
        }
        fprintf(debug, "\n");
        free(tempGrid);
    }
    free_path(&path);
}

int main() {
    int width, height;
    u64 seed;
    float noise_thresh, noise_scale;
    FILE* debug;
    FILE* output;

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
        width = 1000;
        height = 1000;
        seed = 3;
        noise_thresh = -0.25f;
        noise_scale = 1.0f;
    }
    
    if (DEBUG) {
        debug = fopen("debug.txt", "w");
    }
    output = fopen("output.txt", "w");

    Graph graph = generate_graph(debug, width, height, seed, noise_thresh, noise_scale);

    if (DEBUG) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                fprintf(debug, "%d ", graph[x + width * y]);
            }
            fprintf(debug, "\n");
        }
        fprintf(debug, "\n");
    }

    fprintf(output, "astar:");
    profile_pathfinder(debug, output, astar, graph, width, height, (Coord) { 0, 0 }, (Coord) { width - 1, height - 1 });

    //TODO: cellular automata comparison

    if (DEBUG) {
        fclose(debug);
    }
    fclose(output);
    free(graph);
    return 0;
}

