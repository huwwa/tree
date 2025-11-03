#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "util.c"

#define ST_DATA static

#define add_path(path) dynarray_add(&list.paths, &list.nb_paths, strdup(path));

ST_DATA int filter_hidden(const struct dirent *ent);
ST_DATA void traverse(const char *path, const char *indent, int depth);
ST_DATA void tree_print(const char *path);

ST_DATA void parse_args(int argc, char **argv);
ST_DATA void usage(void);
ST_DATA void run(void);
ST_DATA void cleanup(void);

/* Globals */
ST_DATA unsigned int file_count = 0;
ST_DATA unsigned int dir_count = 0;
ST_DATA int max_depth = -1;
ST_DATA List list = {0};

static inline int filter_hidden(const struct dirent *e)
{
    return (e->d_name[0] != '.');
}

#endif //TREE_H
