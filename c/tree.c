#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define BLUE   "\033[1;34m"
#define GREEN  "\033[1;32m"
#define RESET  "\033[0m"

typedef struct {
    unsigned int files;
    unsigned int dirs;
}  Stats;

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Paths;

static void die(const char *fmt, ...);
static char *pathjoin(const char *root, const char *leaf);
static char *indentjoin(const char *a, const char *b);
static int ishidden(const struct dirent *ent);
static void cprint(const char *color_code, const char *restrict fmt, ...);
static void traverse(const char *path, const char *indent, int depth);
static void treeprint(const char *path);
static void info(void);
static void usage(void);
static void load(char *path);
static void run(void);
static void cleanup(void);

static int max_depth = -1;
static Stats stats = {0};
static Paths paths = {0};

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}

char *pathjoin(const char *root, const char *leaf)
{
    bool needsep;
    size_t rootlen, size;
    char *ret;

    rootlen = strlen(root);
    needsep = (rootlen > 0 && root[rootlen - 1] != '/');
    size = rootlen + needsep + strlen(leaf) + 1;
    ret = malloc(size);
    assert(ret && "no enough memory.");

    snprintf(ret, size, "%s%s%s", root, needsep ? "/" : "", leaf);
    return ret;
}

char *indentjoin(const char *a, const char *b)
{
    char *ret;
    size_t size;

    size = strlen(a) + strlen(b) + 1;
    ret = malloc(size);
    assert(ret && "no enough memory.");
    snprintf(ret, size, "%s%s", a, b);
    return ret;
}

int ishidden(const struct dirent *e)
{
    return (e->d_name[0] != '.');
}

void cprint(const char *color_code, const char *restrict fmt, ...)
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

void traverse(const char *path, const char *indent, int depth)
{
    int n;
    struct dirent **list;

    if (max_depth != -1 && depth > max_depth)
        return;

    n = scandir(path, &list, ishidden, alphasort);
    if (n == -1)
        die("could not open `%s`: %s\n", path, strerror(errno));

    struct dirent *e;
    char *subindent, *full;
    for (int i = 0; i < n; ++i) {

        printf("%s%s", indent, (i == n-1) ? "└── " : "├── ");

        e = list[i];
        full = pathjoin(path, e->d_name);
        if (e->d_type == DT_DIR) {
            cprint(BLUE, "%s\n", e->d_name);
            stats.dirs++;

            subindent = indentjoin(indent, (i == n-1) ? "    " : "│   ");
            traverse(full, subindent, depth + 1);
            free(subindent);
        } else {
            cprint((access(full, X_OK) == 0) ? GREEN : "", "%s\n", e->d_name);
            stats.files++;
        }

        free(full);
        free(e);
    }
    free(list);
}

void treeprint(const char *path)
{
    if (access(path, F_OK) != 0)
        die("could not open `%s`: %s.\n", path, strerror(errno));

    cprint(BLUE, "%s\n", path);
    traverse(path, "", 1);
}

void info(void)
{
    printf("\n%u %s, %u %s\n", 
            stats.dirs , (stats.dirs > 1) ? "directories" : "directory", 
            stats.files, (stats.files > 1) ? "files" : "file");
}

void usage(void)
{
    fprintf(stderr,
            "Usage: tree [OPTION]... [PATH]...\n\n"
            "Options:\n"
            "  -L level     Limit the display depth of the tree\n"
            "  -h           Show this help message\n");
    exit(EXIT_SUCCESS);
}


void load(char *path)
{
    stats.dirs++;
    if ((paths.count+1) * sizeof(paths.items[0]) >= paths.capacity)
        if (!(paths.items = realloc(paths.items, (paths.capacity += 1024))))
            die("could not reallocate %zu bytes.", paths.capacity);

    paths.items[paths.count++] = path;
}

void run(void)
{
    for (size_t i = 0; i < paths.count; ++i)
        treeprint(paths.items[i]);
}

void cleanup()
{
    free(paths.items);
}

int main(int argc, char *argv[])
{
    if (argc == 1)
        load(".");

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    usage(); break;
                case 'L':
                    if (argv[++i]) {
                        max_depth = atoi(argv[i]);
                        if (max_depth < 1) {
                            die("tree: invalid level for -L\n");
                        }
                    } else {
                        die("tree: missing argument for -L\n");
                    } break;
                default:
                    die("tree: invalid option '%s'\n"
                            "Try 'tree -h' for help\n", argv[i]);
                    break;
            }
        } else {
            load(argv[i]);
        }
    }

    if (argc == 3 && max_depth > 0)
        load(".");

    run();
    info();

    cleanup();
    return EXIT_SUCCESS;
}
