#include "tree.h"

void traverse(const char *path, const char *indent, int depth)
{
    int n;
    struct dirent **list;

    if (max_depth != -1 && depth > max_depth)
        return;

    n = scandir(path, &list, filter_hidden, alphasort);
    if (n == -1)
        die("could not open '%s': %s\n", path, strerror(errno));

    struct stat sb;
    struct dirent *e;
    char *subindent, *absolute_path;
    for (int i = 0; i < n; ++i) {

        printf("%s%s", indent, (i == n-1) ? "└── " : "├── ");

        e = list[i];
        absolute_path = path_join(path, e->d_name);
        if (stat(absolute_path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            cprint(BLUE, "%s\n", e->d_name);
            dir_count++;

            subindent = join(indent, (i == n-1) ? "    " : "│   ");
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

void tree_print(const char *path)
{
    if (access(path, F_OK) != 0)
        die("could not open '%s': %s.\n", path, strerror(errno));

    dir_count++;
    cprint(BLUE, "%s\n", path);
    traverse(path, "", 1);
}

void usage(void)
{
    die("tree " VERSION " (c) 2025 huwwana@gmail.com\n" \
            "Usage: tree [OPTION]... [PATH]...\n\n"
            "Options:\n"
            "  -L level     Limit the display depth of the tree\n"
            "  -h           Show this help message\n");
}

void parse_args(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
	if (argv[i][0] == '-') {
	    switch (argv[i][1]) {
		case 'h':
                    usage();
                case 'L':
                    if (i+1 < argc) {
                        char *end;
                        max_depth = (int) strtol(argv[++i], &end, 10);
                        if (*end != '\0' || max_depth < 1) {
                            die("tree: invalid level for -L\n");
                        }
                    } else {
                        die("tree: missing argument for -L\n");
                    }
                    break;
                default:
                    die("tree: invalid option '%s'\n"
                            "Try 'tree -h' for help\n", argv[i]);
                    break;
            }
        } else {
            add_path(argv[i]);
        }
    }
}

void run(void)
{
    if (list.nb_paths == 0)
        add_path(".");

    for (int i = 0; i < list.nb_paths; ++i)
        tree_print(list.paths[i]);

    printf("\n%u %s, %u %s\n",
            dir_count , (dir_count > 1) ? "directories" : "directory",
            file_count, (file_count > 1) ? "files" : "file");
}

void cleanup(void)
{
    dynarray_reset(&list.paths, &list.nb_paths);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    run();
    cleanup();
    return EXIT_SUCCESS;
}
