#include "util.h"

PUB_FUNC void dynarray_add(void *ptab, int *nb_ptr, void *data)
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

PUB_FUNC void dynarray_reset(void *pp, int *n)
{
    void **p;
    for (p = *(void***)pp; *n; ++p, --*n)
        if (*p)
            free(*p);
    free(*(void**)pp);
    *(void**)pp = NULL;
}

PUB_FUNC void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    exit(EXIT_FAILURE);
}

PUB_FUNC char *path_join(const char *root, const char *leaf)
{
    int needsep, rootlen, size;
    char *ret;

    rootlen = strlen(root);
    needsep = (rootlen > 0 && root[rootlen - 1] != '/');
    size = rootlen + needsep + strlen(leaf) + 1;

    if(!(ret = malloc(size)))
        die("failed to allocate `%zu`.\n", size);

    snprintf(ret, size, "%s%s%s", root, needsep ? "/" : "", leaf);
    return ret;
}

PUB_FUNC char *join(const char *a, const char *b)
{
    char *ret;
    size_t size;

    size = strlen(a) + strlen(b) + 1;
    if(!(ret = malloc(size)))
        die("failed to allocate `%zu`.\n", size);
    snprintf(ret, size, "%s%s", a, b);
    return ret;
}

PUB_FUNC void cprint(const char *color_code, const char *restrict fmt, ...)
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
