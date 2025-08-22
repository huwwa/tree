BUILDIR := ./build

jtree: $(BUILDIR)/src/Tree.class
ctree: bin/ctree

$(BUILDIR)/src/Tree.class: ./java/src/Tree.java
	javac -d $(BUILDIR) -cp ./java/src $<

bin/ctree: ./c/tree.c
	cc -Wall -Wextra -ggdb -fsanitize=address -o $@ $<

clean:
	rm -rf $(BUILDIR)

.PHONY: clean jtree ctree
