#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>

#include "tree.h"

bool valid_path(const char *path)
{
    return (access(path, F_OK) == 0);
}

bool is_executable(const char *path)
{
    return (access(path, X_OK) == 0);
}

char *join_path(const char *root, const char *leaf)
{
    size_t root_len = strlen(root);
    size_t leaf_len = strlen(leaf);
    char *ret = malloc(root_len + leaf_len + 2);
    memcpy(ret, root, root_len);
    ret[root_len] = '/';
    memcpy(ret + root_len + 1, leaf, leaf_len + 1);
    return ret;
}

char *join_indent(const char *a, const char *b)
{
    size_t a_len = strlen(a);
    size_t b_len = strlen(b);
    char *ret = malloc(a_len + b_len + 1);
    memcpy(ret, a, a_len);
    memcpy(ret, b, b_len + 1);
    return ret;
}

int filter(const struct dirent *ent)
{
    return (ent->d_name[0] != '.');
}

void print_color(const char *color_code, const char *restrict fmt, ...)
{
    if (isatty(STDOUT_FILENO)) {
        fprintf(stdout, "%s", color_code);

        va_list ap;
        va_start(ap, fmt);
        vfprintf(stdout, fmt, ap);
        va_end(ap);

        fprintf(stdout,  "\033[0m");
    } else {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stdout, fmt, ap);
        va_end(ap);
    }
}

void traverse(const char *indent, const char *path, Counter *counter)
{
    struct dirent **namelist;
    int n = scandir(path, &namelist, filter, alphasort);
    if (n == -1) {
        fprintf(stderr, "could not open %s: %s\n", path, strerror(errno));
        exit(1);
    }

    for (int i = 0; i < n; ++i) {
        struct dirent *entry = namelist[i];
        bool is_last = (i == n - 1);
        printf("%s%s", indent, is_last ? "└── " : "├── ");

        char *absolute_path = join_path(path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            print_blue("%s\n", entry->d_name);
            counter->dir_count++;
            char *subindent = join_indent(indent, is_last ? "    " : "│   ");
            traverse(subindent, absolute_path, counter);
            free(subindent);
        } else {
            if (is_executable(absolute_path)) 
                print_green("%s\n", entry->d_name);
            else
                printf("%s\n", entry->d_name);
            counter->file_count++;
        }

        free(absolute_path);
        free(entry);
    }
    free(namelist);
}

void print_tree(const char *path)
{
    if (valid_path(path)) {
        print_blue("%s\n", path);
        Counter counter = {.dir_count = 1};
        traverse("", path, &counter);
        size_t dir_count = counter.dir_count;
        size_t file_count = counter.file_count;
        char *s1 = (dir_count > 1) ? "directories" : "directory";
        char *s2 = (file_count > 1) ? "files" : "file";
        printf("\n%zu %s, %zu %s\n", dir_count, s1, file_count, s2);
    } else {
        fprintf(stderr, "could not open %s: %s.\n", path, strerror(errno));
        exit(64);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "USAGE:\n\t %s [path]\n", argv[0]);
        exit(64);
    } 

    print_tree(argv[1]);
    return 0;
}
