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
    } else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: cjit commit <message>\n");
            return 1;
        }
        static int commit_id = 1;
        FILE *commits = fopen(".cjit/commits.txt", "a");
        fprintf(commits, "%d|%s|", commit_id, argv[2]);

        char staged_file[128];
        FILE *staging = fopen(".cjit/staging.txt", "r");
        while (fgets(staged_file, 128, staging)) {
            staged_file[strlen(staged_file)-1] = '\0';
            fprintf(commits, "%s,", staged_file);

            char obj_path[256];
            sprintf(obj_path, ".cjit/objects/%d_%s", commit_id, staged_file);
            FILE *src = fopen(staged_file, "r");
            FILE *dest = fopen(obj_path, "w");
            char ch;
            while ((ch = fgetc(src)) != EOF) {
                fputc(ch, dest);
            }
            fclose(src);
            fclose(dest);
        }
        fprintf(commits, "\n");
        fclose(staging);
        fclose(commits);

        FILE *clear = fopen(".cjit/staging.txt", "w");
        fclose(clear);
        printf("Committed: %s (id: %d)\n", argv[2], commit_id);
        commit_id++;
    }

    return 0;
}
