#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "tree.h"

Counter counter = {.dir_count = 1};

bool is_path_valid(const char *path)
{
    return (access(path, F_OK) == 0);
}

bool is_executable(const char *path)
{
    return (access(path, X_OK) == 0);
}

char *create_full_path(const char *root, const char *leaf)
{
    size_t root_len = strlen(root);
    size_t leaf_len = strlen(leaf);
    char *ret;
    if (root[root_len] == '/' || *leaf == '/') {
        ret = malloc(root_len + leaf_len + 1);
        assert(ret && "BUY MORE RAM!!\n");
        memcpy(ret, root, root_len);
        memcpy(ret + root_len, leaf, leaf_len + 1);
    } else {
        ret = malloc(root_len + leaf_len + 2);
        assert(ret && "BUY MORE RAM!!\n");
        memcpy(ret, root, root_len);
        ret[root_len] = '/';
        memcpy(ret + root_len + 1, leaf, leaf_len + 1);
    }
    return ret;
}

char *create_next_indentation(const char *a, const char *b)
{
    size_t a_len = strlen(a);
    size_t b_len = strlen(b);
    char *ret = malloc(a_len + b_len + 1);
    assert(ret && "BUY MORE RAM!!\n");
    memcpy(ret, a, a_len);
    memcpy(ret + a_len, b, b_len + 1);
    return ret;
}

int filter_non_hidden_files(const struct dirent *ent)
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

        fprintf(stdout, RESET);
    } else {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stdout, fmt, ap);
        va_end(ap);
    }
}

void traverse(const char *indent, const char *path)
{
    struct dirent **namelist;
    int n = scandir(path, &namelist, filter_non_hidden_files, alphasort);
    if (n == -1) {
        fprintf(stderr, "could not open %s: %s\n", path, strerror(errno));
        exit(1);
    }

    for (int i = 0; i < n; ++i) {
        struct dirent *entry = namelist[i];
        bool is_last = (i == n - 1);
        printf("%s%s", indent, is_last ? "└── " : "├── ");

        char *absolute_path = create_full_path(path, entry->d_name);
        if (entry->d_type == DT_DIR) {
            print_blue("%s\n", entry->d_name);
            counter.dir_count++;
            char *subindent = create_next_indentation(indent, is_last ? "    " : "│   ");
            traverse(subindent, absolute_path);
            free(subindent);
        } else {
            if (is_executable(absolute_path)) 
                print_green("%s\n", entry->d_name);
            else
                printf("%s\n", entry->d_name);
            counter.file_count++;
        }

        free(absolute_path);
        free(entry);
    }
    free(namelist);
}

void print_directory_tree(const char *path)
{
    if (is_path_valid(path)) {
        print_blue("%s\n", path);
        traverse("", path);
    } else {
        fprintf(stderr, "could not open %s: %s.\n", path, strerror(errno));
        exit(64);
    }
}

void print_count(void)
{
    size_t dir_count = counter.dir_count;
    size_t file_count = counter.file_count;
    char *s1 = (dir_count > 1) ? "directories" : "directory";
    char *s2 = (file_count > 1) ? "files" : "file";
    printf("\n%zu %s, %zu %s\n", dir_count, s1, file_count, s2);
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        print_directory_tree(".");
    } else {
        for (int i = 1; i < argc; ++i)
            print_directory_tree(argv[i]);
    }

    print_count();
    return 0;
}
