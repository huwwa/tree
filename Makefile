include config.mk

tree: tree.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -rf tree

.PHONY: tree clean
