#include "bv.h"
#include "ht.h"
#include "node.h"
#include <stdio.h>
#include "salts.h"
#include "speck.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Struct for the Bit Vector
//
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

// Creates the BitVector struct given the
// amount of bits wanted
//
// Credits: Based off the bv_create in
//          asgn 6.
//
BitVector *bv_create(uint32_t length) {
    uint32_t mod = length % 8;
    uint32_t div = length / 8;
    if (mod > 0) {
        div += 1;
    }
    length = div * 8;
    BitVector *bv = (BitVector *) calloc(length, sizeof(BitVector));
    if (bv) {
        bv->length = length;
        bv->vector = (uint8_t *) calloc(length, sizeof(uint8_t));
        if (bv->vector) {
            return bv;
        }
    }
    free(bv);
    return NULL;
}

// Function that deletes and frees any
// allocated space within the BitVector
//
// Credits: Based off the bv_delete in
//          asgn 6.
//
void bv_delete(BitVector **bv) {
    if (bv) {
        free((*bv)->vector);
        free(*bv);
        *bv = NULL;
    }
}

// Returns the amount of bits within
// the bit vector.
//
// Credits: Based off the bv8.h file
//          the git lab resources folder
//          for CSE13s
//
uint32_t bv_length(BitVector *bv) {
    if (bv) {
        return bv->length;
    }
    return 0;
}

// Sets the given bit index of the bit
// vector to 1.
//
// Credits: Based off the bv8.h file
//          the git lab resources folder
//          for CSE13s
//
bool bv_set_bit(BitVector *bv, uint32_t i) {
    if (bv && bv->vector) {
        return bv->vector[i / 8] |= ((uint8_t) 0x1 << i % 8);
    }
    return false;
}

// Function that sets the bit at the given
// bit index of the bit vector to 0.
//
// Credits: Based off the bv8.h file
//          the git lab resources folder
//          for CSE13s
//
bool bv_clr_bit(BitVector *bv, uint32_t i) {
    if (bv && bv->vector) {
        return bv->vector[i / 8] &= ~((uint8_t) 0x1 << i % 8);
    }
    return false;
}

// Function that returns whether the bit
// value of the given index is a 1 or 0.
//
// Credits: Based off the bv8.h file
//          the git lab resources folder
//          for CSE13s.
//
bool bv_get_bit(BitVector *bv, uint32_t i) {
    return (bv->vector[i / 8] >> i % 8) & 0x1;
}

// Print statement used for debugging
//
void bv_print(BitVector *bv) {
    for (uint64_t i = 0; i < (bv->length * 8); i++) {
        printf("index %lu bit value %d \n", i, bv_get_bit(bv, i));
    }
}
