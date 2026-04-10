#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Commit {
    int id;
    char message[100];
};

int next_commit_id = 1;

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

void do_commit(char *message) {
    struct Commit *c = malloc(sizeof(struct Commit));
    c->id = next_commit_id++;
    strcpy(c->message, message);

    FILE *f = fopen(".cjit/commits.txt", "a");
    fprintf(f, "%d|%s\n", c->id, c->message);

    char staged_file[128];
    FILE *staging = fopen(".cjit/staging.txt", "r");
    FILE *commits_dir = fopen(".cjit/commits.txt", "r+");

    while (fgets(staged_file, 128, staging)) {
        staged_file[strlen(staged_file)-1] = '\0';
        FILE *src = fopen(staged_file, "r");
        if (src) {
            char path[256];
            sprintf(path, ".cjit/%d_%s", c->id, staged_file);
            FILE *dest = fopen(path, "w");
            char ch;
            while ((ch = fgetc(src)) != EOF) {
                fputc(ch, dest);
            }
        }
    }

    fclose(staging);
    FILE *clear = fopen(".cjit/staging.txt", "w");
    fclose(clear);

    printf("Committed: %s (id: %d)\n", c->message, c->id);
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
    } else if (strcmp(argv[1], "commit") == 0) {
        if (argc < 3) {
            printf("Usage: cjit commit <message>\n");
            return 1;
        }
        do_commit(argv[2]);
    } else {
        printf("Unknown command: %s\n", argv[1]);
    }

    return 0;
}
