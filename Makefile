CC ?= cc
CFLAGS = -Wall -Wextra -O2

tree: tree.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf tree

.PHONY: clean
