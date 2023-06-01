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
#include "lookahead.h"
#include "bmp.h"
//#include "mincut.h"

typedef Path (*PathfinderFunction)(Graph, Pos, Pos);

//generates a graph of walls and empty nodes, the empty nodes are always connected, and the top left and bottom right right corners are always empty
Graph generate_graph(FILE* debug, int width, int height, u64 seed, float noise_thresh, float noise_scale) {
    Graph graph = create_graph(width, height);
    u64* rng = init_rng(seed);

    fill_with_noise(graph, rng, noise_thresh, noise_scale);
    set_cell(graph, (Pos) { 0, 0 }, 0);
    set_cell(graph, (Pos) { width - 1, height - 1 }, 0);

    if (DEBUG) {
        write_bmp("before_connection.bmp", graph);
    }

    connect_graph(graph, rng);

    if (DEBUG) {
        write_bmp("graph.bmp", graph);
    }

    return graph;
}

//profiling function written by chatgpt
void profile_pathfinder(FILE* debug, FILE* output, FILE* image_output, PathfinderFunction f, Graph graph, Pos start, Pos end) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;
    double interval;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time); //start the timer

    Path path = f(graph, start, end);

    QueryPerformanceCounter(&end_time); //stop the timer
    interval = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    fprintf(output, "time %fs\n", interval);
    fprintf(output, "path length %d\n", path.num_steps);
    fprintf(output, "nodes pushed %d\n", path.nodes_pushed);
    fprintf(output, "largest heap %d\n", path.largest_heap);

    if (DEBUG) {
        // Create a temporary grid for printing.
        Graph temp = create_graph(graph.width, graph.height);
        memcpy(temp.cells, graph.cells, graph.width * graph.height * sizeof(Cell));

        // Replace path nodes in the temporary grid with a special marker
        for (int i = 0; i < path.num_steps; i++) {
            Pos c = path.steps[i];
            set_cell(temp, c, 255);
        }

        write_bmp(image_output, temp);
        free_graph(temp);
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
        width = 2000;
        height = 2000;
        seed = 3;
        noise_thresh = -0.15f;
        noise_scale = 0.1f;
    }
    
    if (DEBUG) {
        debug = fopen("debug.txt", "w");
    }
    output = fopen("output.txt", "w");

    Graph graph = generate_graph(debug, width, height, seed, noise_thresh, noise_scale);

    fprintf(output, "astar:\n");
    profile_pathfinder(debug, output, "astar.bmp", astar, graph, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });
    fprintf(output, "\nlookahead:\n");
    profile_pathfinder(debug, output, "lookahead.bmp", lookahead, graph, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });

    //TODO: cellular automata comparison

    if (DEBUG) {
        fclose(debug);
    }
    fclose(output);
    free_graph(graph);
    return 0;
}

