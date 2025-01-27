#include "node.h"
#include "ht.h"
#include "text.h"
#include "metric.h"
#include "parser.h"
#include "bv.h"
#include "bf.h"
#include "pq.h"
#include "text.h"
#include <stdio.h>
#include <regex.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE 2048

// Usage page that directs the user on how to properly
// use the program
//
// Credits: Based of the Collaztz.c usage page. The
//          acutal Synopsis and etc is from the
//          encrypt executable from the resources file
//
void usage(char *name) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "    Identifies the author of some anonymous text passed through stdin.\n"
        "    Basically computes k-nearest neighbors, where k = 1 by default.\n"
        "    The number of noise words that are removed is tunable.\n"
        "    Uses either Euclidean, Manhattan, or cosine distance as metric.\n"
        "\n"
        "    USAGE\n"
        "        %s [OPTIONS]\n"
        "\n"
        "    OPTIONS\n"
        "       -d database     Database of authors and texts [default: lib.db]\n"
        "       -k matches      Set number of top matches to display [default: 5]\n"
        "       -n noisefile    Set file of words to ignore [default: noise.txt]\n"
        "       -l limit        Set noise word limit [default: 100]\n"
        "       -e              Set distance metric as Euclidean distance [default]\n"
        "       -m              Set distance metric as Manhattan distance\n"
        "       -c              Set distance metric as cosine distance\n"
        "       -v              Enable verbose printing of program run\n"
        "       -h              Display program help and usage\n",
        name);
}

// The main program that will take in a file directory from
// stdin and will use it to compare with other text files
// from a specified directory. The original file will be compared
// to the texts in the inputed file directory and will return the
// specified amount of mathces with the closest resemblance to the
// inputted file. When it returns the matches it will print the
// authors names and the distance between the two files.
//
// Credits: I got how to remove the newline from the user
//          gecko10000#7137 in the cse13s asgn 7 discussion
//          discord.
//          I got how to allocate memory for a char pointer from
//          eugene and ben from the cse13s discord server
//
int main(int argc, char **argv) {
    int opt = 0;
    char *database = "lib.db";
    char *noise_input = "noise.txt";
    FILE *fillin = stdin;
    uint32_t match = 5;
    noiselimit = 100;
    uint32_t mets = 0;
    while ((opt = getopt(argc, argv, "h, d:, n:, k:, l:, e, m, c")) != -1) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        case 'd': database = optarg; break;
        case 'n': noise_input = optarg; break;
        case 'k': match = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'l': noiselimit = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'e': mets = EUCLIDEAN; break;
        case 'm': mets = 1; break;
        case 'c': mets = 2; break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }
    uint32_t n = 0;
    char *author_name = (char *) malloc(MAX_SIZE);
    char *path = (char *) malloc(MAX_SIZE);
    double distance = 0;
    FILE *noise_file = fopen(noise_input, "r");
    FILE *data = fopen(database, "r");
    FILE *pile = NULL;
    Text *noise = text_create(noise_file, NULL);
    Text *text = text_create(fillin, noise);
    fscanf(data, "%d \n", &n);
    PriorityQueue *pq = pq_create(n);
    for (uint32_t i = 0; i < n; i += 1) {
        fgets(author_name, MAX_SIZE, data);
        fgets(path, MAX_SIZE, data);
        author_name[strlen(author_name) - 1] = '\0';
        path[strlen(path) - 1] = '\0';
        pile = fopen(path, "r");
        if (pile != NULL) {
            Text *tex = text_create(pile, noise);
            distance = text_dist(tex, text, EUCLIDEAN + mets);
            enqueue(pq, author_name, distance);
            text_delete(&tex);
        }
    }
    printf("Top %u, metric: %s, noise limit: %u\n", match, metric_names[mets], noiselimit);
    for (uint32_t i = 1; match >= i; i++) {
        if (pq_empty(pq)) {
            break;
        }
        dequeue(pq, &author_name, &distance);
        printf("%u) %s [%0.15f]\n", i, author_name, distance);
    }
    free(author_name);
    free(path);
    pq_delete(&pq);
    text_delete(&text);
    text_delete(&noise);
}
