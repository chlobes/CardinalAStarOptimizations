#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

//chatgpt
#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} BMPInfoHeader;
#pragma pack(pop)

void write_bmp(char* filename, Graph graph, Cell prune_generations) {
    int row_size = (graph.width * 3 + 3) & (~3); // 24-bit bitmap, padded to multiple of 4 bytes
    int data_size = row_size * graph.height;
    int file_size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + data_size;

    FILE* file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Failed to open file for writing %s\n", filename);
        return;
    }

    // Write bitmap header
    BMPHeader header = { 0x4D42, file_size, 0, 0, sizeof(BMPHeader) + sizeof(BMPInfoHeader) };
    fwrite(&header, sizeof(BMPHeader), 1, file);

    // Write bitmap info header
    BMPInfoHeader info_header = { sizeof(BMPInfoHeader), graph.width, graph.height, 1, 24, 0, data_size, 0, 0, 0, 0 };
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);

    // Write pixel data
    uint8_t* row_buffer = malloc(row_size * sizeof(uint8_t));
    for (int y = graph.height - 1; y >= 0; y--)
    {
        for (int x = 0; x < graph.width; x++)
        {
            Cell pixel = cell(graph, (Pos) { x, y });
            uint8_t red = 0, green = 0, blue = 0;

            if (pixel == 0) { //empty
                red = 255, green = 255, blue = 255;
            }
            else if (pixel == 1) { //wall
            }
            else if (pixel == 255) { //path
                red = 255;
            }
            else if (pixel < 6) { //checked
                blue = 255;
            }
            else { //pruned
                pixel -= 6;
                if (pixel < prune_generations / 2) {
                    green = 255 - (int)pixel * 510 / (int)prune_generations;
                } else {
                    red = ((int)pixel - (int)prune_generations / 2) * 510 / (int)prune_generations;
                    blue = ((int)pixel - (int)prune_generations / 2) * 510 / (int)prune_generations;
                }
            }

            int offset = x * 3;
            row_buffer[offset + 0] = blue;
            row_buffer[offset + 1] = green;
            row_buffer[offset + 2] = red;
            for (int i = graph.width * 3; i < row_size; i++) row_buffer[i] = 0;
        }

        fwrite(row_buffer, sizeof(uint8_t), row_size, file);
    }

    free(row_buffer);
    fclose(file);
}