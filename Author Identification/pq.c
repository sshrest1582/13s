#include "pq.h"
#include "bv.h"
#include "text.h"
#include "bf.h"
#include "ht.h"
#include "node.h"
#include "parser.h"
#include "salts.h"
#include "speck.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Struct that is used to create and Auth node
//
// Credits: Based off the node struct in asgn6 and
//          asgn7
//
typedef struct Auth {
    char *name;
    double dista;
} Auth;

// Struct that is used to create the priority queue
//
// Credits: Based of the asgn6 priority queue
//
struct PriorityQueue {
    uint32_t head;
    uint32_t tail;
    uint32_t capacity;
    struct Auth **A;
};

// Creates an Auth type node that holds an authors name
// and their distance
//
Auth *Author_create(char *name, double distances) {
    Auth *A = (Auth *) malloc(sizeof(Auth));
    A->name = strdup(name);
    A->dista = distances;
    return A;
}

// Deletes and frees all the allocated space within
// an Auth type node
//
void Author_delete(Auth **A) {
    free((*A)->name);
    free(*A);
    *A = NULL;
}

// Insertion sort algorithm that sorts the Auth nodes
// in the queue from the higest to lowest distances
//
// Credits: Based off the asgn3 insertion sort
//          algorithm.
//
void insertion_sort(PriorityQueue *q) {
    for (uint32_t i = 1; i < q->head; i += 1) {
        uint32_t j = i;
        Auth *tmp = (*q).A[i];
        while (j > 0 && tmp->dista > q->A[j - 1]->dista) {
            (*q).A[j] = (*q).A[j - 1];
            j -= 1;
        }
        (*q).A[j] = tmp;
    }
    return;
}

// Creates a priority queue that holds an authors name
// and correlated distance value and sorts it from lowest
// highest priority
//
// Credits: Based off the priority queue in asgn 6
//
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->head = 0;
        q->tail = 0;
        q->capacity = capacity;
        q->A = (Auth **) malloc(q->capacity * sizeof(Auth *));
        if (q->A) {
            return q;
        }
        free(q->A);
        free(q);
    }
    return (PriorityQueue *) NULL;
}

// Deletes all the Auth type nodes in the queue
// and frees any allocated space within it
//
void pq_delete(PriorityQueue **q) {
    if (*q) {
        for (uint32_t i = 0; i < pq_size(*q); i++) {
            Author_delete(&(*q)->A[i]);
        }
        free((*q)->A);
        free(*q);
        *q = NULL;
    }
}

// Returns whether the queue is empty or not
//
bool pq_empty(PriorityQueue *q) {
    if (q) {
        if (q->head == q->tail) {
            return true;
        }
    }
    return false;
}

// Returns whether the queue is full or not
//
bool pq_full(PriorityQueue *q) {
    uint32_t cap = q->capacity;
    if (q) {
        if (q->head == cap) {
            return true;
        }
    }
    return false;
}

// Returns the current size or total number of
// elemeents within the queue
//
uint32_t pq_size(PriorityQueue *q) {
    return q->head;
}

// Enqueues the inputed author name and associate distance
// into the priority queue
//
bool enqueue(PriorityQueue *q, char *author, double dist) {
    if (q) {
        if (pq_full(q)) {
            return false;
        }
        Auth *a = Author_create(author, dist);
        q->A[q->head] = a;
        q->head += 1;
        insertion_sort(q);
        return true;
    } else {
        return false;
    }
}

// Function that sends the highest priority values to the
// inputted values and drecrements the head
//
// Credits: Based off the asgn 6 priority queue.
//
bool dequeue(PriorityQueue *q, char **author, double *dist) {
    if (q) {
        if (pq_empty(q)) {
            return false;
        }
        q->head -= 1;
        *author = q->A[q->head]->name;
        *dist = q->A[q->head]->dista;
        return true;
    } else {
        return false;
    }
}

// Print statement used for debugging
//
void pq_print(PriorityQueue *q) {
    printf("%u num\n", q->head);
    for (uint32_t i = 0; i < q->head; i++) {
        printf("%d Author name %s, distance %f \n ", i, q->A[i]->name, q->A[i]->dista);
    }
    printf("head %u \n tail %u \n size %u \n", q->head, q->tail, q->capacity);
}
