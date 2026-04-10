#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_repo() {
    system("mkdir .cjit");
    FILE *f1 = fopen(".cjit/commits.txt", "w");
    FILE *f2 = fopen(".cjit/staging.txt", "w");
    fclose(f1);
    fclose(f2);
    printf("Initialized empty CJIT repository\n");
}

void add_file(char *filename) {
    char buffer[256];
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: cannot open %s\n", filename);
        return;
    }

    while (fgets(buffer, 256, f) != NULL) {
    }

    FILE *staging = fopen(".cjit/staging.txt", "a");
    fprintf(staging, "%s\n", filename);

    printf("Staged: %s\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: cjit <command>\n");
        return 1;
    }

    if (strcmp(argv[1], "init") == 0) {
        init_repo();
    } else if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            printf("Usage: cjit add <filename>\n");
            return 1;
        }
        add_file(argv[2]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}
