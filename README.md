A simple command-line utility to display a tree-like directory structure, similar to the `tree` command. 

### Build

```console
# Java version
make jtre && ./bin/jtree [path]
# C version
make ctree && ./bin/ctree [path]
```

# Sample Output:
``` console
./ctree .
 ./bin/ctree .
.
├── LICENSE
├── Makefile
├── bin
│   ├── ctree
│   └── jtree
├── c
│   ├── tree.c
│   └── tree.h
└── java
    └── src
        └── Tree.java

5 directories, 7 files
```
