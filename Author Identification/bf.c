#include "bv.h"
#include "bf.h"
#include "ht.h"
#include "node.h"
#include "salts.h"
#include "speck.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Struct for the BloomFilter.
//
// Credits: Based off the struct of the
//          Bloom filter in asgn 7 document
struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

// Function that creates the BloomFilter
// struct
//
BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(BloomFilter));

    if (bf) {
        bf->primary[0] = SALT_PRIMARY_LO;
        bf->primary[1] = SALT_PRIMARY_HI;
        bf->secondary[0] = SALT_SECONDARY_LO;
        bf->secondary[1] = SALT_SECONDARY_HI;
        bf->tertiary[0] = SALT_TERTIARY_LO;
        bf->tertiary[1] = SALT_TERTIARY_HI;
        bf->filter = bv_create(size);
        if (bf->filter) {
            return bf;
        }
    }
    free(bf);
    return NULL;
}

// Function that deletes and frees any
// allocated space within the BloomFilter
//
void bf_delete(BloomFilter **bf) {
    if (*bf) {
        bv_delete((&(*bf)->filter));
        free(*bf);
        *bf = NULL;
    }
}

// Returns the size of the Bloom Filter
//
uint32_t bf_size(BloomFilter *bf) {
    if (bf) {
        return 8 * bv_length(bf->filter);
    }
    return 1;
}

// Inserts a words bit value into the bit
// vector.
//
void bf_insert(BloomFilter *bf, char *word) {
    uint64_t bits = hash(bf->primary, word);
    bits = bits % bf_size(bf);
    bv_set_bit(bf->filter, bits);
    bits = hash(bf->secondary, word);
    bits = bits % bf_size(bf);
    bv_set_bit(bf->filter, bits);
    bits = hash(bf->tertiary, word);
    bits = bits % bf_size(bf);
    bv_set_bit(bf->filter, bits);
}

// Returns whether or not the words bit value
// is within the bloom filters bit vectors
//
bool bf_probe(BloomFilter *bf, char *word) {
    uint32_t pits = hash(bf->primary, word);
    pits = pits % bf_size(bf);
    uint32_t sits = hash(bf->secondary, word);
    sits = sits % bf_size(bf);
    uint32_t bits = hash(bf->tertiary, word);
    bits = bits % bf_size(bf);
    if (bv_get_bit(bf->filter, pits) == true && bv_get_bit(bf->filter, sits)
        && bv_get_bit(bf->filter, bits) == true) {
        return true;
    }
    return false;
}

// Useful print statesmnet for debugging
//
void bf_print(BloomFilter *bf) {
    uint32_t num = bv_length(bf->filter);
    for (uint32_t i = 0; i < num; i++) {
        bv_print(bf->filter);
    }
}
