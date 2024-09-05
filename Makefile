CC = clang
CFLAGS = -Wall -Wextra -Werror -pedantic
all: encode decode

io.o: io.c io.h
	clang $(CFLAGS) -c io.c

trie.o: trie.c trie.h
	clang $(CFLAGS) -c trie.c

word.o: word.c word.h
	clang $(CFLAGS) -c word.c

encode:	encode.o	io.o	trie.o	word.o
	$(CC) -o encode encode.o io.o trie.o word.o -lm

decode:	decode.o	io.o	trie.o	word.o
	$(CC) -o decode decode.o io.o trie.o word.o -lm

clean	:
	rm -rf encode decode encode.o decode.o io.o trie.o word.o infer-out
infer	:
	make clean; infer-capture -- make; infer-analyze -- make
