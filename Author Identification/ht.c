#include "ht.h"
#include "node.h"
#include "salts.h"
#include "speck.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Struct for the Hash table
//
struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

// Struct for the Hash Table Iterator
//
struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
};

// Function that creates the HashTable and allocates the
// needed space for it.
//
HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(HashTable));
    if (ht) {
        ht->salt[0] = SALT_HASHTABLE_LO;
        ht->salt[1] = SALT_HASHTABLE_HI;
        ht->size = size;
        ht->slots = (Node **) calloc(ht->size, sizeof(Node *));
        if (ht->slots) {
            return ht;
        }
        free(ht);
    }
    return (HashTable *) NULL;
}

// Deletes and frees the alloacted space of the given
// hash table
//
void ht_delete(HashTable **ht) {
    if (*ht) {
        HashTableIterator *hti = hti_create(*ht);
        Node *n = NULL;
        while ((n = ht_iter(hti)) != NULL) {
            node_delete(&n);
        }
        free((*ht)->slots);
        hti_delete(&hti);
        free(*ht);
        *ht = NULL;
    }
}

// Returns the size of the hash table
//
uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

// Returns the node if the given word is within the
// hash table.
//
// Credits: Based off the code of the ht_search in the
//          lecture slides for hash
//
Node *ht_lookup(HashTable *ht, char *word) {
    if (ht) {
        uint32_t count = 0;
        uint32_t index = hash(ht->salt, word) % ht_size(ht);
        while (count < ht_size(ht)) {
            Node *noder = ht->slots[index];
            if (noder && strcmp(noder->word, word) == 0) {
                return ht->slots[index];
            }
            index = (index + 1) % ht_size(ht);
            count += 1;
        }
    }
    return NULL;
}

// Inserts a word into the hash table if it's not
// already in there if not increase the nodes count
//
// Credits: Based off the code given in the lecture
//          slides for hash.
//
Node *ht_insert(HashTable *ht, char *word) {
    uint32_t count = 0;
    uint32_t index = hash(ht->salt, word);
    index = index % ht_size(ht);
    while (count < ht_size(ht)) {
        if (ht->slots[index] == NULL) {
            ht->slots[index] = node_create(word);
            (ht->slots[index])->count = 1;
            return ht->slots[index];
        }
        if (strcmp((ht->slots[index])->word, word) == 0) {
            (ht->slots[index])->count = (ht->slots[index])->count + 1;
            return ht->slots[index];
        }
        count += 1;
        index = (index + 1) % ht_size(ht);
    }
    return NULL;
}

// Print function used for debugging
//
void ht_print(HashTable *ht) {
    HashTableIterator *hti = hti_create(ht);
    Node *n = NULL;
    while ((n = ht_iter(hti)) != NULL) {
        printf("%s\n", n->word);
    }
    hti_delete(&hti);
}

// Creates a Hash Table iterator that iterates
// through the given hash table.
//
HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(HashTableIterator));
    hti->table = ht;
    hti->slot = 0;
    return hti;
}

// Frees and deletes and allocated space within
// the given hash table iterator.
//
void hti_delete(HashTableIterator **hti) {
    if (*hti) {
        free(*hti);
        *hti = NULL;
    }
}

// Function that iterates through the given hash
// table until it has found a valid node and returns
// said node.
//
Node *ht_iter(HashTableIterator *hti) {
    if (hti->slot >= hti->table->size) {
        return NULL;
    }
    while (hti->table->slots[hti->slot] == NULL && hti->table->size > hti->slot) {
        hti->slot += 1;
    }
    Node *iter = hti->table->slots[hti->slot];
    hti->slot += 1;
    return iter;
}
