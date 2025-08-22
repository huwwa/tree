#ifndef TREE_H
#define TREE_H

#define GREEN  "\033[1;32m"
#define BLUE   "\033[1;34m"
#define RESET  "\033[0m"

#define print_blue(fmt, ...) \
    print_color(BLUE, fmt, ##__VA_ARGS__)

#define print_green(fmt, ...) \
    print_color(GREEN, fmt, ##__VA_ARGS__)

typedef struct {
    size_t file_count;
    size_t dir_count;
} Counter;

bool valid_path(const char *path);
bool is_executable(const char *path);
char *join_path(const char *root, const char *leaf);
char *join_indent(const char *a, const char *b);
int filter(const struct dirent *ent);
void print_color(const char *color_code, const char *restrict fmt, ...);
void traverse(const char *indent, const char *path, Counter *counter);
void print_tree(const char *path);

#endif //TREE_H
