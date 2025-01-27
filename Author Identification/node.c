#include "node.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Function that creates the Node and
// allocates the needed space for the
// word.
//
// Credits: Based off the node.c file
//          from asgn 6.
//
Node *node_create(char *word) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->word = strdup(word);
    n->count = 0;
    return n;
}

// Function that deletes and frees
// any space within the node
//
// Credits: Based off the node.c file
//          from asgn 6
//
void node_delete(Node **n) {
    free((*n)->word);
    free(*n);
    *n = NULL;
}

// Print function used for debugging
//
void node_print(Node *n) {
    printf("word: %s number: %u\n", n->word, n->count);
}
