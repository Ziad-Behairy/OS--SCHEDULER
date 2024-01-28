#include <stddef.h>
typedef struct {
    int priority;
    int data;
} node_t;

typedef struct {
    node_t *nodes;
    int len;
    int size;
} heap_t;

void push (heap_t *h, int priority, int pid_n) {
    if (h->len + 1 >= h->size) {
        h->size = h->size ? h->size * 2 : 4;
        h->nodes = (node_t *)realloc(h->nodes, h->size * sizeof (node_t));
    }
    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2;
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = pid_n;
    h->len++;
}

int pop (heap_t *h) {
    int i, j, k;
    if (!h->len) {
        return NULL;
    }
    int data = h->nodes[1].data;
    
    h->nodes[1] = h->nodes[h->len];
    
    h->len--;
    
    i = 1;
    while (i!=h->len+1) {
        k = h->len+1;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) {
            k = j + 1;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    return data;
}