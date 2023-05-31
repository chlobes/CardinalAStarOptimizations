#pragma once
#include <stdint.h>

typedef uint64_t u64;

u64* init_rng(u64 seed);

int gen_random(u64* state);