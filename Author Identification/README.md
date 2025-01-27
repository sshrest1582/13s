#Assighnment 7: Author Identification (README.md)

## Short desription:
    This program has one main executable called identify. This executable
    takes in a specified file path from stdin and uses this fiel path to 
    to create a Text type variable that will be used to compare the similarities
    between the inputted text and an entire database of authors and texts. The 
    program will print the authors that are within the database that closest 
    resembles the writer of the text received from stdin. 
    
    This takes program in a at most 6 inputs in the getopt at once. The input 
    "-e" allows the program to complete a Euclidean distance formula, whereas 
    "-m" will do a Manhattan distance formula, and "-c" will perform a Cosine 
    distance formula. The formuals will be used to calculte the similarities 
    between texts and the authors. The program will only perform one distance 
    formula at a time. The user can modify the noise limit which is the amount 
    words that will filtered out of each text such words can include "I", "a",
    "the", and so on. The noise limit is preset to be 100. The user can also 
    dictate which txt file they would like to use as a noise filter with the 
    input "-n file.txt". The amount of matches or authors with the similar 
    word or writing patterns can be specified with the input "-k number", but
    the amount of matches is preset to 5. With the input "-d database.db" the 
    user will be able to decide which databse the program grabs text file paths
    and author names from.

## Build
    To build manually, type "clang -Wall -Wextra -Werror -Wpedantic -o node.o 
    ht.o speck.o bv.o bf.o parser.o text.o pq.o identifiy.o identify.c" on the
    command line. Alternatively, type "make" on the command line with the 
    Makefile provided in the directory.

## Running
   ./identify -k 10 -d small.db < texts/william-shakespeare.txt

## Cleaning
    To clean manually, type "rm -f node.o ht.o speck.o bv.o bf.o parser.o
    text.o pq.o identifiy.o identify. Alternatively type "make clean" on 
    the command line, using Makefile provided.

## Error Handling:
    If the user input an improper input they will direclty sent to the usage page
    and the program will terminate.

    If the the program is unable to open a file then it will skip over to the next 
    path and try to open that one.

    In the case that any of the structs such as node_create, pq_create, text_create
    are unable to properly create the wanted struct pointer it will free any space 
    allocated and return NULL.

