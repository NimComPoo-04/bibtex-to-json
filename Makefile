CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, obj/%.o, $(SRC))

all: build b2j

build:
	mkdir -p obj

b2j: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

run: b2j
	./b2j cite.bib
	@echo
	@echo EXIT CODE: $$?

clean:
	rm -rf obj b2j
