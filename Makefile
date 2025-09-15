BUILDIR := ./build
CFLAGS :=  -Wall -Wextra -O3
#CFLAGS :=  -Wall -Wextra -ggdb -fsanitize=address

ctree: bin/ctree
jtree: $(BUILDIR)/src/Tree.class

$(BUILDIR)/src/Tree.class: ./java/src/Tree.java
	javac -d $(BUILDIR) -cp ./java/src $<

bin/ctree: ./c/tree.c
	cc $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILDIR)

.PHONY: clean jtree ctree
