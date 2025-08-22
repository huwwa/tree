import java.io.IOException;
import java.nio.file.*;
import java.util.Comparator;
import java.util.stream.Stream;

public class ModernTree {

    private static final String BLUE = "\u001B[34m";
    private static final String GREEN = "\u001B[32m";
    private static final String RESET = "\u001B[0m";

    private static int fileCount = 0;
    private static int dirCount = 0;

    public static void main(String[] args) {
        if (args.length > 1) {
            System.err.println("USAGE:\n\t java ModernTree [path]");
            System.exit(64);
        }

        Path startPath = Paths.get(args.length == 0 ? "." : args[0]);

        if (!Files.exists(startPath)) {
            System.err.println("Error: Path does not exist.");
            System.exit(1);
        }

        System.out.println(BLUE + startPath.toString() + RESET);
        try {
            printTree(startPath, "");
        } catch (IOException e) {
            System.err.println("Error traversing path: " + e.getMessage());
            System.exit(1);
        }

        System.out.printf("%n%d directories, %d files%n", dirCount, fileCount);
    }

    private static void printTree(Path directory, String indent) throws IOException {
        try (Stream<Path> stream = Files.list(directory)) {
            Path[] sortedPaths = stream
                .sorted(Comparator.comparing(Path::getFileName))
                .toArray(Path[]::new);

            for (int i = 0; i < sortedPaths.length; i++) {
                Path currentPath = sortedPaths[i];
                boolean isLast = (i == sortedPaths.length - 1);

                System.out.print(indent + (isLast ? "└── " : "├── "));

                if (Files.isDirectory(currentPath)) {
                    dirCount++;
                    System.out.println(BLUE + currentPath.getFileName() + RESET);
                    printTree(currentPath, indent + (isLast ? "    " : "│   "));
                } else {
                    fileCount++;
                    if (Files.isExecutable(currentPath)) {
                        System.out.println(GREEN + currentPath.getFileName() + RESET);
                    } else {
                        System.out.println(currentPath.getFileName());
                    }
                }
            }
        }
    }
}
