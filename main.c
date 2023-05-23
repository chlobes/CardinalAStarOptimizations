#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "graph.h"
#include "noise.h"

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
    width = 100;
    height = 100;
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

    fclose(fp);
    free(graph);
    return 0;
}