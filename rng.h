#pragma once

#include <stdint.h>
#include <stdlib.h>


typedef uint64_t u64;

//an xorshift pseudorandom generator I wrote in rust a long time ago, translated to C

u64* init_rng(u64 seed) {
    seed += (SIZE_MAX / 7U * 4U); //small seeds cause the first few values to be wierd so we avoid them
    u64* state = (u64*)malloc(5 * sizeof(u64));
    state[0] = seed;
    state[1] = seed % 865941;
    state[2] = seed % 45129;
    state[3] = seed % 963;
    state[4] = seed % 7437;
    return state;
}

int gen_random(u64* state) {
    u64 t = state[3];
    t ^= t >> 2;
    t ^= t << 1;
    state[3] = state[2]; state[2] = state[1]; state[1] = state[0];
    u64 s = state[0];
    t ^= s;
    t ^= s << 4;
    state[0] = t;
    state[4] += 362437;
    return (int)((t + state[4]) % INT_MAX);
}