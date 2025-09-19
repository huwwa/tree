#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#define BLUE   "\033[1;34m"
#define GREEN  "\033[1;32m"
#define RESET  "\033[0m"

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Paths;

static void die(const char *fmt, ...);
static char *pathjoin(const char *root, const char *leaf);
static char *indentjoin(const char *a, const char *b);
static int filter_hidden(const struct dirent *ent);
static void cprint(const char *color_code, const char *restrict fmt, ...);
static void traverse(const char *path, const char *indent, int depth);
static void treeprint(const char *path);
static void info(void);
static void usage(void);
static void add_path(char *path);
static void run(void);
static void cleanup(void);

/* Globals */
static unsigned int file_count = 0;
static unsigned int dir_count = 0;
static int max_depth = -1;
static Paths paths = {0};

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
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

    if(!(ret = malloc(size)))
        die("failed to allocate `%zu`.\n", size);

    snprintf(ret, size, "%s%s%s", root, needsep ? "/" : "", leaf);
    return ret;
}

char *indentjoin(const char *a, const char *b)
{
    char *ret;
    size_t size;

    size = strlen(a) + strlen(b) + 1;
    if(!(ret = malloc(size)))
        die("failed to allocate `%zu`.\n", size);
    snprintf(ret, size, "%s%s", a, b);
    return ret;
}

int filter_hidden(const struct dirent *e)
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

    n = scandir(path, &list, filter_hidden, alphasort);
    if (n == -1)
        die("could not open `%s`: %s\n", path, strerror(errno));

    struct stat sb;
    struct dirent *e;
    char *subindent, *absolute_path;
    for (int i = 0; i < n; ++i) {

        printf("%s%s", indent, (i == n-1) ? "└── " : "├── ");

        e = list[i];
        absolute_path = pathjoin(path, e->d_name);
        if (stat(absolute_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            cprint(BLUE, "%s\n", e->d_name);
            dir_count++;

            subindent = indentjoin(indent, (i == n-1) ? "    " : "│   ");
            traverse(absolute_path, subindent, depth + 1);
            free(subindent);
        } else {
            cprint((access(absolute_path, X_OK) == 0) ? GREEN : "", "%s\n", e->d_name);
            file_count++;
        }

        free(absolute_path);
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
            dir_count , (dir_count > 1) ? "directories" : "directory", 
            file_count, (file_count > 1) ? "files" : "file");
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


void add_path(char *path)
{
    dir_count++;
    if ((paths.count+1) >= paths.capacity)
        if (!(paths.items = realloc(paths.items, sizeof(*paths.items)*(paths.capacity += 1024))))
            die("cannot realloc %zu\n", sizeof(*paths.items)*paths.capacity);

    paths.items[paths.count++] = path;
}

void run(void)
{
    for (size_t i = 0; i < paths.count; ++i)
        treeprint(paths.items[i]);
}

void cleanup()
{
    /*no strdup is used!*/
    free(paths.items);
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    usage(); break;
                case 'L':
                    if (i+1 < argc) {
                        char *end;
                        max_depth = (int) strtol(argv[++i], &end, 10);
                        if (*end != '\0' || max_depth < 1) {
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
            add_path(argv[i]);
        }
    }

    if (paths.count == 0)
        add_path(".");

    run();
    info();

    cleanup();
    return EXIT_SUCCESS;
}
