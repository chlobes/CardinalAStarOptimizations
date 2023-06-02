#pragma once
#include "graph.h"
#include "rng.h"

//checks if all empty nodes of the graph are connected and randomly adds connections until they are
void connect_graph(Graph graph);