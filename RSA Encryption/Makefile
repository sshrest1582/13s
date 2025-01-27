CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -g $(shell pkg-config --cflags gmp)
OFUNCT =  randstate.o numtheory.o rsa.o  
VALORANT = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose
LFLAGS = $(shell pkg-config --libs gmp)

.PHONY: all scan-build valgrind

all: keygen encrypt decrypt

keygen: $(OFUNCT) keygen.o
	$(CC)  -o keygen $(OFUNCT) keygen.o $(LFLAGS)

keygen.o: keygen.c
	$(CC)  $(CFLAGS)  -c keygen.c

encrypt: $(OFUNCT) encrypt.o
	$(CC)  -o encrypt $(OFUNCT) encrypt.o $(LFLAGS)

encrypt.o: encrypt.c
	$(CC)  $(CFLAGS)  -c encrypt.c

decrypt: $(OFUNCT) decrypt.o
	$(CC)  -o decrypt $(OFUNCT) decrypt.o $(LFLAGS)

decrypt.o: decrypt.c
	$(CC)  $(CFLAGS)  -c decrypt.c

clean:
	rm -f *.o keygen encrypt decrypt rsa.priv rsa.pub

scan-build: clean
	scan-build --use-cc=$(CC) make

valgrind: encrypt 
	valgrind $(VALORANT) ./encrypt -v -i jason.txt -o hobo.txt  

format:
	clang-format -i -style=file *.[ch]
