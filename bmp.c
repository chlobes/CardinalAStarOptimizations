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

void write_bmp(char* filename, Graph graph)
{
    int rowSize = (graph.width * 3 + 3) & (~3); // 24-bit bitmap, padded to multiple of 4 bytes
    int dataSize = rowSize * graph.height;
    int fileSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + dataSize;

    FILE* file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Failed to open file for writing.\n");
        return;
    }

    // Write bitmap header
    BMPHeader header = { 0x4D42, fileSize, 0, 0, sizeof(BMPHeader) + sizeof(BMPInfoHeader) };
    fwrite(&header, sizeof(BMPHeader), 1, file);

    // Write bitmap info header
    BMPInfoHeader infoHeader = { sizeof(BMPInfoHeader), graph.width, graph.height, 1, 24, 0, dataSize, 0, 0, 0, 0 };
    fwrite(&infoHeader, sizeof(BMPInfoHeader), 1, file);

    // Write pixel data
    uint8_t* rowBuffer = malloc(rowSize * sizeof(uint8_t));
    for (int y = graph.height - 1; y >= 0; y--)
    {
        for (int x = 0; x < graph.width; x++)
        {
            int pixel = cell(graph, (Pos) { x, y });
            uint8_t red = 0, green = 0, blue = 0;

            if (pixel == 0) {
                red = 255, green = 255, blue = 255;
            }
            else if (pixel == 1) {
            } else if (pixel == 255) {
                red = 255;
            } else if (pixel == 2) {
                green = 255;
            }

            int offset = x * 3;
            rowBuffer[offset + 0] = blue;
            rowBuffer[offset + 1] = green;
            rowBuffer[offset + 2] = red;
        }

        fwrite(rowBuffer, sizeof(uint8_t), rowSize, file);
    }

    free(rowBuffer);
    fclose(file);
}