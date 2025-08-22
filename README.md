# Tree

A simple reimplementation of the `tree` command in **C** and **Java**.  
It displays a tree-like directory structure from the command line.

## Sample Output

```console
$ ./bin/ctree .
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
````

## Quick Start

### Java version

```console
make jtree && ./bin/jtree [path]
```

### C version

```console
make ctree && ./bin/ctree [path]
```

## License

This project is licensed under the **BSD 2-Clause License** – see the [LICENSE](LICENSE) file for details.
