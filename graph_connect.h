#pragma once
#include "graph.h"
#include "rng.h"

//search for a random point of desired type, return INT_MAX if we can't find one
Coord find(Graph graph, int width, int height, Cell target, int start_x, int start_y);

//starts at a point and fills all zeroes with twos to mark a region
void flood(Graph graph, int width, int height, int x, int y);

//checks if all empty nodes of the graph are connected and randomly adds connections until they are
void connect_graph(Graph graph, int width, int height, u64* rng);