#pragma once
#include "graph.h"
#include "rng.h"

void fill_with_noise(Graph graph, int width, int height, u64* rng, float thresh, float scale);