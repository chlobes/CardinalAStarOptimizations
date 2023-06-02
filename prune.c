#pragma once
#include "prune.h"
#include "graph.h"
#include "bmp.h"
#include <stdlib.h>
#include <stdio.h>

const unsigned char lut[256] = {
	0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0,
	1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
	0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0,
	0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1
};

void prune_cell(Graph graph, Pos* stack, int* stack_size, Pos p, Cell generation) {
	if (cell(graph, p)) return; //even though we checked before calling, we have to recheck, since we might get called twice on the same cell
	if (p.x == 0 && p.y == 0) return;
	if (p.x == graph.width - 1 && p.y == graph.height - 1) return;
	unsigned char byte = 0;

	Pos neighbours[8] = {
		{p.x, p.y - 1}, {p.x - 1, p.y}, {p.x + 1, p.y}, {p.x, p.y + 1},
		{p.x - 1, p.y - 1}, {p.x + 1, p.y - 1}, {p.x - 1, p.y + 1}, {p.x + 1, p.y + 1}
	};

	for (int i = 0; i < 8; ++i) {
		if (neighbours[i].x < 0 || neighbours[i].y < 0 || neighbours[i].x >= graph.width
		|| neighbours[i].y >= graph.height || cell(graph, neighbours[i])) byte |= 1 << i; //we treat tiles outside the map as walls
	}

	if (lut[byte]) {
		set_cell(graph, p, generation);

		for (int i = 0; i < 8; ++i) {
			if (neighbours[i].x < 0 || neighbours[i].y < 0 || neighbours[i].x >= graph.width || neighbours[i].y >= graph.height) continue;
			if (!cell(graph, neighbours[i])) stack[(*stack_size)++] = neighbours[i];
		}
	}
}

int prune_graph(Graph graph) {
	//TODO: use a dynamic vector so we don't risk buffer overrun
	Pos* stack1 = malloc(graph.width * graph.height * sizeof(Pos));
	Pos* stack2 = malloc(graph.width * graph.height * sizeof(Pos) / 2);
	int stack_size1 = 0;
	int stack_size2 = 0;
	Cell generation = 6;

	for (int x = 0; x < graph.width; x++) {
		for (int y = 0; y < graph.height; y++) {
			if (!cell(graph, (Pos) { x, y })) prune_cell(graph, stack1, &stack_size1, (Pos) { x, y }, generation);
		}
	}

	while (stack_size1) {
		generation = (generation + 1) % 255;
		while (stack_size1) prune_cell(graph, stack2, &stack_size2, stack1[--stack_size1], generation);
		generation = (generation + 1) % 255;
		while (stack_size2) prune_cell(graph, stack1, &stack_size1, stack2[--stack_size2], generation);
	}

	return generation - 6;
}