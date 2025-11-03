#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>

#define BLUE   "\033[1;34m"
#define GREEN  "\033[1;32m"
#define RESET  "\033[0m"

typedef struct {
    char **paths;
    int nb_paths;
} List;

void die(const char *fmt, ...);
void cprint(const char *code, const char *restrict fmt, ...);
char *path_join(const char *root, const char *leaf);
char *join(const char *a, const char *b);

#endif //UTIL_H
