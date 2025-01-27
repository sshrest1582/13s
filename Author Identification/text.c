#include "bv.h"
#include "text.h"
#include "bf.h"
#include "ht.h"
#include "node.h"
#include "parser.h"
#include "salts.h"
#include "speck.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <regex.h>

#define WORD            "[a-zA-Z]+((-[a-zA-Z]+)|('[a-zA-Z]+))*"
#define lowercase(c)    (c - 'A' + 'a')
#define isupper(c)      (c >= 'A' && c <= 'Z')
#define lowercaseify(c) (isupper(c) ? lowercase(c) : c)

// Number of noise words to filter out.
uint32_t noiselimit;

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

// Function that calcultes the Eulicid Disntance between two
// texts.
//
double euc_dist(Text *text1, Text *text2) {
    double num1 = 0;
    double num2 = 0;
    double total = 0;
    Node *node2 = NULL;
    Node *node1 = NULL;
    HashTableIterator *htiter = hti_create(text1->ht);
    HashTableIterator *hti2 = hti_create(text2->ht);
    while ((node1 = ht_iter(htiter)) != NULL) {
        if (text_contains(text2, node1->word)) {
            num2 = text_frequency(text2, node1->word);
            num1 = text_frequency(text1, node1->word);
            total += (double) pow(num1 - num2, 2);
        } else {
            num1 = text_frequency(text1, node1->word);
            total += (double) pow(num1, 2);
        }
    }
    while ((node2 = ht_iter(hti2)) != NULL) {
        if ((text_contains(text1, node2->word)) == false) {
            num2 = text_frequency(text2, node2->word);
            total += (double) pow(num2, 2);
        }
    }
    hti_delete(&htiter);
    hti_delete(&hti2);
    total = (double) sqrt(total);
    return total;
}

// Finds the cosine distance between two texts
//
double cos_dist(Text *text1, Text *text2) {
    double num1 = 0;
    double num2 = 0;
    double one = 1;
    Node *node1 = NULL;
    HashTableIterator *htiter = hti_create(text1->ht);
    while ((node1 = ht_iter(htiter)) != NULL) {
        if (text_contains(text2, node1->word)) {
            num1 = text_frequency(text1, node1->word);
            num2 = text_frequency(text2, node1->word);
            num1 *= num2;
            one -= num1;
        }
    }
    hti_delete(&htiter);
    return one;
}

// Finds the manhattan distance between two texts
//
double man_dist(Text *text1, Text *text2) {
    double num1 = 0;
    double num2 = 0;
    double total = 0;
    Node *node1 = NULL;
    Node *node2 = NULL;
    HashTableIterator *htiter = hti_create(text1->ht);
    HashTableIterator *hti2 = hti_create(text2->ht);
    while ((node1 = ht_iter(htiter)) != NULL) {
        if (text_contains(text2, node1->word)) {
            num2 = text_frequency(text2, node1->word);
            num1 = text_frequency(text1, node1->word);
            total += fabs(num1 - num2);
        } else {
            num1 = text_frequency(text1, node1->word);
            total += fabs(num1);
        }
    }
    while ((node2 = ht_iter(hti2)) != NULL) {
        if (text_contains(text1, node2->word) == false) {
            num2 = text_frequency(text2, node2->word);
            total += fabs(num2);
        }
    }
    hti_delete(&htiter);
    hti_delete(&hti2);
    return total;
}

// Creates the text variable type and initializes
// all the variables and functions and allocates the
// space of each variable
//
// Credits: I got the lowercaseify function from the
//          discord user ELmer#1515.
//          The regex is from the asgn 7 assignment
//          document
//
Text *text_create(FILE *infile, Text *noise) {
    regex_t re;
    if (regcomp(&re, WORD, REG_EXTENDED)) {
        fprintf(stderr, "Failed to compile regex.\n");
        return NULL;
    }
    Text *text = (Text *) malloc(sizeof(Text));
    if (text) {
        text->word_count = 0;
        text->ht = ht_create(1 << 19);
    }
    if (text == NULL) {
        regfree(&re);
        return NULL;
    }
    if (text->ht) {
        text->bf = bf_create(1 << 21);
    }
    if (text->ht == NULL) {
        regfree(&re);
        free(text);
        return NULL;
    }
    if (text->bf == NULL) {
        regfree(&re);
        free(text->ht);
        free(text);
        return NULL;
    }
    if (text->bf) {
        if (noise == NULL) {
            char *word = NULL;
            while ((word = next_word(infile, &re)) != NULL && text->word_count < noiselimit) {
                for (uint32_t i = 0; i < strlen(word); i++) {
                    word[i] = lowercaseify(word[i]);
                }
                ht_insert(text->ht, word);
                bf_insert(text->bf, word);
                text->word_count += 1;
            }
            regfree(&re);
            return text;
        } else {
            char *word = NULL;
            while ((word = next_word(infile, &re)) != NULL) {
                for (uint32_t i = 0; i < strlen(word); i++) {
                    word[i] = lowercaseify(word[i]);
                }
                if (bf_probe(noise->bf, word) == false) {
                    bf_insert(text->bf, word);
                    ht_insert(text->ht, word);
                    text->word_count += 1;
                }
            }
            regfree(&re);
            return text;
        }
    }
    return NULL;
}

// Deletes and frees all the allocated space
// of the given text pointer
//
void text_delete(Text **text) {
    if (*text) {
        bf_delete(&(*text)->bf);
        ht_delete(&(*text)->ht);
        free(*text);
        *text = NULL;
    }
}

// Finds the texts distance using either the cosine,
// euclidean, or manhattan distance formulas
//
double text_dist(Text *text1, Text *text2, Metric metric) {
    if (metric == MANHATTAN) {
        return man_dist(text1, text2);
    }
    if (metric == EUCLIDEAN) {
        return euc_dist(text1, text2);
    }
    if (metric == COSINE) {
        return cos_dist(text1, text2);
    }
    return 0;
}

// Returns the frequency of a word within an text
// otherwise return 0.
//
double text_frequency(Text *text, char *word) {
    if (text) {
        if (bf_probe(text->bf, word) == false) {
            return 0;
        } else {
            Node *node = ht_lookup(text->ht, word);
            return (double) (node->count) / (text->word_count);
        }
    }
    return 0;
}

// Returns true if the given text contains the
// inputted word
//
bool text_contains(Text *text, char *word) {
    if (text) {
        if (bf_probe(text->bf, word) == false) {
            return false;
        }
        Node *node = ht_lookup(text->ht, word);
        if (node != NULL && node->count != 0) {
            return true;
        }
    }
    return false;
}

// Print function used for debugging.
//
void text_print(Text *text) {
    printf("tex word->count %u\n", text->word_count);
    printf("tex ht_size %u\n", ht_size(text->ht));
    printf("tex bf_size %u\n", bf_size(text->bf));
}
