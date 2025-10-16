CFLAGS :=  -Wall -Wextra -O3
#CFLAGS :=  -Wall -Wextra -ggdb -fsanitize=address

tree: tree.c
	cc $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILDIR)

.PHONY: clean
