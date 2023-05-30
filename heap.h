#include <stdlib.h>

//binary heap implementation for astar's priority queue, credit to chatgpt
//except that it created a max heap when I wanted a min heap because it's silly

typedef struct node {
    int x, y;
    int g;
    int h;
    int f;
    //int steps;
} Node;

typedef struct {
    Node* data;
    int capacity;
    int size;
} Heap;

Heap create_heap(int capacity) {
    Heap heap;
    heap.data = (Node*)malloc(capacity * sizeof(Node));
    heap.capacity = capacity;
    heap.size = 0;
    return heap;
}

void heap_push(Heap* heap, Node node) {
    if (heap->size == heap->capacity) {
        // Heap is full, we need to increase its capacity
        heap->capacity *= 2;
        heap->data = (Node*)realloc(heap->data, heap->capacity * sizeof(Node));
    }
    // Insert the new node at the end of the heap
    int i = heap->size;
    heap->data[i] = node;
    heap->size++;

    // Bubble up
    while (i != 0 && heap->data[i].f < heap->data[(i - 1) / 2].f) {
        Node temp = heap->data[i];
        heap->data[i] = heap->data[(i - 1) / 2];
        heap->data[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

Node heap_pop(Heap* heap) {
    if (heap->size == 0) {
        // Heap is empty
        return (Node) { -1, -1, INT_MAX, INT_MAX, INT_MAX };
    }

    // Get the node with the minimum f value
    Node minNode = heap->data[0];
    // Move the node at the end of the heap to the root
    heap->data[0] = heap->data[heap->size - 1];
    heap->size--;

    // Bubble down
    int i = 0;
    while (1) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < heap->size && heap->data[left].f < heap->data[smallest].f) {
            smallest = left;
        }
        if (right < heap->size && heap->data[right].f < heap->data[smallest].f) {
            smallest = right;
        }
        if (smallest == i) {
            break;
        }
        Node temp = heap->data[i];
        heap->data[i] = heap->data[smallest];
        heap->data[smallest] = temp;
        i = smallest;
    }

    return minNode;
}

void free_heap(Heap* heap) {
    free(heap->data);
}