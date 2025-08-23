package src;

import java.io.IOException;
import java.nio.file.*;
import java.util.Comparator;
import java.util.stream.Stream;

public class Tree {

    private static final String BLUE = "\u001B[34m";
    private static final String GREEN = "\u001B[32m";
    private static final String RESET = "\u001B[0m";

    private static int fileCount = 0;
    private static int dirCount = 0;

    private static void printTree(Path root) throws IOException {
	if (!Files.exists(root)) {
	    System.err.println("Error: can not open: " + root.toString() + " no such file or directory.");
	    System.exit(1);
	}

	System.out.println(BLUE + root.toString() + RESET);
	traverse(root, "");
	System.out.printf("%n%d directories, %d files%n", dirCount, fileCount);
    }

    private static void traverse(Path root, String indent) throws IOException {
	Path[] sortedPaths = Files.list(root)
	    .filter(p -> !p.getFileName().toString().startsWith("."))
	    .sorted(Comparator.comparing(Path::getFileName))
	    .toArray(Path[]::new);

	for (int i = 0; i < sortedPaths.length; i++) {
	    Path currentPath = sortedPaths[i];
	    boolean isLast = (i == sortedPaths.length - 1);

	    System.out.print(indent + (isLast ? "└── " : "├── "));

	    if (Files.isDirectory(currentPath)) {
		dirCount++;
		System.out.println(BLUE + currentPath.getFileName() + RESET);
		traverse(currentPath, indent + (isLast ? "    " : "│   "));
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

    public static void main(String[] args) throws IOException {
	if (args.length > 1) {
	    System.err.println("USAGE:\n\t ./jtree [path]");
	    System.exit(64);
	}

	Path root = Paths.get((args.length == 0) ? "." : args[0]);
	printTree(root);
    }
}
