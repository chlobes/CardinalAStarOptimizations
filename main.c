#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define WRITE_BMP 1

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
#include "prune.h"
//#include "mincut.h"

typedef Path (*PathfinderFunction)(Graph, Pos, Pos);

//generates a graph of walls and empty nodes, the empty nodes are always connected, and the top left and bottom right right corners are always empty
Graph generate_graph(int width, int height, u64 seed, float noise_thresh, float noise_scale) {
    Graph graph = create_graph(width, height);
    u64* rng = init_rng(seed);

    fill_with_noise(graph, rng, noise_thresh, noise_scale);
    set_cell(graph, (Pos) { 0, 0 }, 0);
    set_cell(graph, (Pos) { width - 1, height - 1 }, 0);

    if (WRITE_BMP) write_bmp("before_connection.bmp", graph, 0);

    connect_graph(graph);

    if (WRITE_BMP) write_bmp("graph.bmp", graph, 0);

    return graph;
}

//profiling function written by chatgpt
void profile_pathfinder(FILE* output, char* image_output, PathfinderFunction f, Graph graph, Cell prune_generations, Pos start, Pos end) {
    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;
    double interval;

    Graph closed_set = create_graph(graph.width, graph.height);
    memcpy(closed_set.cells, graph.cells, closed_set.width * closed_set.height * sizeof(Cell));

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time); //start the timer

    Path path = f(closed_set, start, end);

    QueryPerformanceCounter(&end_time); //stop the timer
    interval = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    fprintf(output, "time %fs\n", interval);
    fprintf(output, "path length %d\n", path.num_steps);
    #ifdef PATH_INFO
    fprintf(output, "nodes discovered %d\n", path.nodes_discovered);
    fprintf(output, "nodes pushed %d\n", path.nodes_pushed);
    fprintf(output, "nodes expanded %d\n", path.nodes_expanded);
    fprintf(output, "largest heap %d\n", path.largest_heap);
    #endif

    for (int i = 0; i < path.num_steps; i++) {
        for (int dir = 0; dir < 4; dir++) { //give the path some width
            int dx = (dir % 2) ? 0 : (dir - 1);
            int dy = (dir % 2) ? (dir - 2) : 0;
            Pos p = (Pos){ path.steps[i].x + dx, path.steps[i].y + dy };
            if (p.x < 0 || p.y < 0 || p.x >= closed_set.width || p.y >= closed_set.height) continue;
            set_cell(closed_set, p, 255);
        }
    }

    if (WRITE_BMP) write_bmp(image_output, closed_set, prune_generations);
    free_graph(closed_set);
    free_path(&path);
}

void compare_algos(int width, int height, u64 seed, float noise_thresh, float noise_scale, FILE* output) {
    printf("\ngrid size: (%d %d)\n", width, height);
    printf("params: %d, %f, %f\n", seed, noise_thresh, noise_scale);
    int prune_generations = 0;

    Graph graph = generate_graph(width, height, seed, noise_thresh, noise_scale);

    fprintf(output, "astar:\n");
    profile_pathfinder(output, "astar.bmp", astar, graph, prune_generations, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });
    fprintf(output, "\nlookahead:\n");
    profile_pathfinder(output, "lookahead.bmp", lookahead, graph, prune_generations, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });

    //TODO: we could clean up this duplicated code with some macros
    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;
    double interval;

    Graph closed_set = create_graph(graph.width, graph.height);
    memcpy(closed_set.cells, graph.cells, closed_set.width * closed_set.height * sizeof(Cell));

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time); //start the timer

    prune_generations = prune_graph(graph);

    QueryPerformanceCounter(&end_time); //stop the timer
    interval = (double)(end_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;

    fprintf(output, "\npruned %d generations in %fs\n", prune_generations, interval);

    if (WRITE_BMP) write_bmp("pruned.bmp", graph, prune_generations);

    fprintf(output, "\npruned astar:\n");
    profile_pathfinder(output, "pruned_astar.bmp", astar, graph, prune_generations, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });
    fprintf(output, "\npruned lookahead:\n");
    profile_pathfinder(output, "pruned_lookahead.bmp", lookahead, graph, prune_generations, (Pos) { 0, 0 }, (Pos) { width - 1, height - 1 });

    free_graph(graph);
}

int main() {
    int width, height;
    u64 seed;
    float noise_thresh, noise_scale;
    int prune_generations = 0;
    FILE* output = fopen("output.txt", "w");

    while (1) {
        printf("enter grid width (0 to exit): ");
        scanf("%d", &width);
        if (!width) break;
        printf("enter grid height: ");
        scanf("%d", &height);
        printf("enter rng seed: ");
        scanf("%llu", &seed);
        printf("enter noise threshold (-1 to 1): ");
        scanf("%f", &noise_thresh);
        printf("enter noise scale: ");
        scanf("%f", &noise_scale);
        width += width % 2; //for some reason, malloc crashes when width/height are odd, don't ask me why
        height += height % 2;

        compare_algos(width, height, seed, noise_thresh, noise_scale, output);
    }

    fclose(output);
    
    return 0;
}

