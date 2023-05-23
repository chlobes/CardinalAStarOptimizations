#pragma once

#include <math.h>
#include <stdint.h>
#include <stddef.h>

//translating a noise generator I wrote in rust for a hobby project, with some assistance from chatgpt
//TODO: should probably put this into a C file
//...but what's the point really? using header files directly lets us bypass C's archaic header system
//shared namespaces are fine, plenty of languages do it by default

static const int PERMUTATION_TABLE[256] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30,
    69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94,
    252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136,
    171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229,
    122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
    63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116,
    188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5,
    202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155,
    167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112,
    104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81,
    51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204,
    176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72,
    243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

//we use an xorshift generator to shuffle the array for perlin noise
int gen_random(size_t* state) {
    size_t t = state[3];
    t ^= t >> 2;
    t ^= t << 1;
    state[3] = state[2]; state[2] = state[1]; state[1] = state[0];
    size_t s = state[0];
    t ^= s;
    t ^= s << 4;
    state[0] = t;
    state[4] += 362437;
    return (int)((t + state[4]) % INT_MAX);
}

float lerp(float t, float a, float b) {
    return a * (1.0f - t) + b * t;
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float grad_2d(int hash, float x, float y) {
    switch (hash & 3) {
    case 0: return x + y;
    case 1: return -x + y;
    case 2: return x - y;
    default: return -x - y;
    }
}

float noise_2d(int* perm_table, int x, int y, float scale) {
    float xf = (float)x + 0.479f;
    float yf = (float)y + 0.479f;

    int xi = ((int)floorf(xf)) & 255;
    int yi = ((int)floorf(yf)) & 255;

    xf = xf - floorf(xf);
    yf = yf - floorf(yf);

    if (xf < 0.0f) xf += 1.0f;
    if (yf < 0.0f) yf += 1.0f;

    int a = perm_table[xi] + yi;
    int aa = perm_table[a];
    int ab = perm_table[a + 1];
    int b = perm_table[xi + 1] + yi;
    int ba = perm_table[b];
    int bb = perm_table[b + 1];

    float g[4] = {
        grad_2d(perm_table[aa], xf, yf),
        grad_2d(perm_table[ba], xf - 1.0f, yf),
        grad_2d(perm_table[ab], xf, yf - 1.0f),
        grad_2d(perm_table[bb], xf - 1.0f, yf - 1.0f)
    };

    float u = fade(xf);
    float v = fade(yf);

    float x1 = lerp(u, g[0], g[1]);
    float x2 = lerp(u, g[2], g[3]);

    return lerp(v, x1, x2);
}

//fill the graph with noise between 0 and max
void fill_with_noise(int width, int height, int* graph, int max, size_t seed, float scale) {
    seed += (SIZE_MAX / 7 * 4); //small seeds cause the first few values to be wierd so we avoid them
    size_t xor_state[5] = { seed, seed % 865941, seed % 45129, seed % 963, seed % 7437 };
    int perm_table[512] = { 0 };

    for (int i = 0; i < 256; i++) {
        perm_table[i] = PERMUTATION_TABLE[i];
    }

    //using the modern Fisher-Yates shuffle
    for (int i = 0; i < 255; i++) { //we go up to the second last index, 254
        int m = 256 - i;
        int j = (gen_random(&xor_state[0]) % m) + i; //i <= j < 256
        int tmp = perm_table[i];
        perm_table[i] = perm_table[j];
        perm_table[j] = tmp;
    }

    for (int i = 0; i < 256; i++) { //duplicate perm table to account for hash overflow
        perm_table[i + 256] = perm_table[i];
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            float noise = noise_2d(&perm_table[0], x, y, scale);
            int scaled = (int)fmaxf((noise + 1.0f) / 2.0f * ((float)max + 0.999f), 1.0f);
            graph[y * width + x] = scaled;
        }
    }
}