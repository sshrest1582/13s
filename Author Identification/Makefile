CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -g $(shell pkg-config --cflags gmp)
OFUNCT =   node.o ht.o speck.o bv.o bf.o parser.o text.o pq.o -lm 
VALORANT = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
LFLAGS = $(shell pkg-config --libs gmp)

.PHONY: all scan-build valgrind

all: identify

identify: $(OFUNCT) identify.o
	$(CC) $(LFLAGS) -o identify $(OFUNCT) identify.o

identify.o: identify.c
	$(CC)  $(CFLAGS)  -c identify.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

ht.o: ht.c
	$(CC) $(CFLAGS) -c ht.c

bf.o: bf.c
	$(CC) $(CFLAGS) -c bf.c

bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

text.o: text.c
	$(CC) $(CFLAGS) -c text.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

speck.o: speck.c
	$(CC) $(CFLAGS) -c speck.c

clean:
	rm -f *.o identify

scan-build: clean
	scan-build --use-cc=$(CC) make

valgrind: identify
	valgrind $(VALORANT) ./identify -k 5 -d small.db < texts/william-shakespeare.txt 

format:
	        clang-format -i -style=file *.[ch]
