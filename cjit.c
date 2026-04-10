#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_repo() {
    system("mkdir .cjit");
    system("mkdir .cjit/objects");
    FILE *f1 = fopen(".cjit/commits.txt", "w");
    FILE *f2 = fopen(".cjit/staging.txt", "w");
    fclose(f1);
    fclose(f2);
    printf("Initialized empty CJIT repository\n");
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
        FILE *f = fopen(argv[2], "r");
        if (!f) {
            printf("Error: cannot open %s\n", argv[2]);
            return 1;
        }
        fclose(f);
        FILE *staging = fopen(".cjit/staging.txt", "a");
        fprintf(staging, "%s\n", argv[2]);
        fclose(staging);
        printf("Staged: %s\n", argv[2]);
    }

    return 0;
}
