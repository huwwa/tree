#include <stdio.h>
#include <stdlib.h>
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
    char **paths;
    int nb_paths;
} Paths;

/* Globals */
static unsigned int nb_files;
static unsigned int nb_dirs;
static int max_depth = -1;
static Paths list;

static const char help[] =
    "tree (c) 2025 huwwana@gmail.com\n"
    "Usage: tree [OPTION]... [PATH]...\n\n"
    "Options:\n"
    "  -L level     Limit the display depth of the tree\n"
    "  -h           Show this help message\n";

void dynarray_add(void *ptab, int *nb_ptr, void *data)
{
    int nb, nb_alloc;
    void **pp;

    nb = *nb_ptr;
    pp = *(void ***)ptab;
    /* every power of two we double array size */
    if ((nb & (nb - 1)) == 0) {
        nb_alloc = !(nb) ? 1 : nb * 2;
        pp = realloc(pp, nb_alloc * sizeof(void *));
        *(void***)ptab = pp;
    }
    pp[nb++] = data;
    *nb_ptr = nb;
}

void dynarray_reset(void *pp, int *n)
{
    void **p;
    for (p = *(void***)pp; *n; ++p, --*n)
        if (*p)
            free(*p);
    free(*(void**)pp);
    *(void**)pp = NULL;
}

void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}
char *path_join(const char *parent, const char *child)
{
    int sep;
    size_t size;
    char *r;

    size = strlen(parent);
    sep = (size > 0 && parent[size - 1] != '/') ? 1 : 0;
    size += sep + strlen(child) + 1;
    if(!(r = malloc(size)))
        die("failed to allocate `%zu`.\n", size);
    snprintf(r, size, "%s%s%s", parent, sep ? "/" : "", child);
    return r;
}

char *join(const char *a, const char *b)
{
    size_t n;
    char *r;

    n = strlen(a) + strlen(b) + 1;
    if(!(r = malloc(n)))
        die("failed to allocate `%zu`.\n", n);
    snprintf(r, n, "%s%s", a, b);
    return r;
}

static inline int filter_hidden(const struct dirent *e)
{
    return (e->d_name[0] != '.');
}

void cprintf(const char *color_code, const char *restrict fmt, ...)
{
    va_list ap;
    if (isatty(STDOUT_FILENO)) {
        fprintf(stdout, "%s", color_code);

        va_start(ap, fmt);
        vfprintf(stdout, fmt, ap);
        va_end(ap);

        fprintf(stdout, RESET);
    } else {
        va_start(ap, fmt);
        vfprintf(stdout, fmt, ap);
        va_end(ap);
    }
}

void traverse(const char *path, const char *indent, int depth)
{
    int n;
    struct stat sb;
    struct dirent *e;
    struct dirent **et;
    char *subindent, *full_path;

    if (max_depth > 0 && depth > max_depth)
        return;

    n = scandir(path, &et, filter_hidden, alphasort);

    for (int i = 0; i < n; ++i) {
        e = et[i];
        full_path = path_join(path, e->d_name);

        printf("%s%s", indent, (i == n-1) ? "└── " : "├── ");

        if (stat(full_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            nb_dirs++;
            cprintf(BLUE, "%s\n", e->d_name);

            subindent = join(indent, (i == n-1) ? "    " : "│   ");
            traverse(full_path, subindent, depth + 1);
            free(subindent);
        } else {
            if (access(full_path, X_OK) == 0)
                cprintf(GREEN, "%s\n", e->d_name);
            else
                printf("%s\n", e->d_name);
            nb_files++;
        }
        free(full_path);
    }
    free(et);
}

void tree_print(const char *path)
{
    if (access(path, F_OK) != 0)
        die("could not open '%s': %s.\n", path, strerror(errno));

    nb_dirs++;
    cprintf(BLUE, "%s\n", path);
    traverse(path, "", 1);
}

int main(int argc, char *argv[])
{
    char *endptr;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            dynarray_add(&list.paths, &list.nb_paths, strdup(argv[i]));
        } else {
            switch (argv[i][1]) {
                case 'L':
                    if (i+1>= argc)
                        die("tree: missing argument for -L\n");
                    max_depth = (int) strtol(argv[++i], &endptr, 10);
                    if (*endptr != '\0' || max_depth < 1)
                        die("tree: invalid level for -L\n");
                    break;
                case 'h':
                default:
                    die(help);
                    break;
            }
        }
    }

    /* default to current directory */
    if (list.nb_paths == 0)
        dynarray_add(&list.paths, &list.nb_paths, strdup("."));

    for (int i = 0; i < list.nb_paths; ++i)
        tree_print(list.paths[i]);

    printf("\n%u %s, %u %s\n",
            nb_dirs , (nb_dirs > 1) ? "directories" : "directory",
            nb_files, (nb_files > 1) ? "files" : "file");

    dynarray_reset(&list.paths, &list.nb_paths);
    return EXIT_SUCCESS;
}
