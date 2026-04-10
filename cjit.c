#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Commit {
    int id;
    char message[100];
    char files[10][100];
    int file_count;
};

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
    } else if (strcmp(argv[1], "checkout") == 0) {
        if (argc < 3) {
            printf("Usage: cjit checkout <commit_id>\n");
            return 1;
        }
        int target_id = atoi(argv[2]);
        FILE *commits = fopen(".cjit/commits.txt", "r");
        char line[512];
        int found = 0;
        while (fgets(line, 512, commits)) {
            int id;
            sscanf(line, "%d|", &id);
            if (id == target_id) {
                found = 1;
                char *files_start = strchr(line, '|');
                files_start = strchr(files_start + 1, '|');
                if (files_start) {
                    files_start++;
                    char *files = strtok(files_start, ",");
                    while (files && strcmp(files, "\n") != 0) {
                        char obj_path[256];
                        sprintf(obj_path, ".cjit/objects/%d_%s", target_id, files);
                        FILE *src = fopen(obj_path, "r");
                        FILE *dest = fopen(files, "w");
                        char ch;
                        while ((ch = fgetc(src)) != EOF) {
                            fputc(ch, dest);
                        }
                        fclose(src);
                        fclose(dest);
                        printf("Restored: %s\n", files);
                        files = strtok(NULL, ",");
                    }
                }
                break;
            }
        }
        fclose(commits);
        if (!found) {
            printf("Commit %d not found\n", target_id);
        }
    } else if (strcmp(argv[1], "log") == 0) {
        FILE *commits = fopen(".cjit/commits.txt", "r");
        if (!commits) {
            printf("No commits found\n");
            return 1;
        }
        char line[512];
        printf("Commit history:\n");
        while (fgets(line, 512, commits)) {
            int id;
            char msg[100];
            sscanf(line, "%d|%99s|", &id, msg);
            printf("  Commit %d: %s\n", id, msg);
        }
        fclose(commits);
    } else if (strcmp(argv[1], "diff") == 0) {
        if (argc < 4) {
            printf("Usage: cjit diff <commit1> <commit2>\n");
            return 1;
        }
        int id1 = atoi(argv[2]);
        int id2 = atoi(argv[3]);
        printf("Diff between commit %d and commit %d:\n", id1, id2);

        char file1[256], file2[256];
        sprintf(file1, ".cjit/objects/%d_test.txt", id1);
        sprintf(file2, ".cjit/objects/%d_test.txt", id2);

        FILE *f1 = fopen(file1, "r");
        FILE *f2 = fopen(file2, "r");

        if (!f1 || !f2) {
            printf("Cannot open object files for comparison\n");
            return 1;
        }

        char line1[256], line2[256];
        int line_num = 1;
        int diff_found = 0;

        while (fgets(line1, 256, f1) || fgets(line2, 256, f2)) {
            if (strcmp(line1, line2) != 0) {
                printf("Line %d:\n", line_num);
                printf("  [%d] %s", id1, line1);
                printf("  [%d] %s", id2, line2);
                diff_found = 1;
            }
            line_num++;
            memset(line1, 0, 256);
            memset(line2, 0, 256);
        }

        if (!diff_found) {
            printf("  No differences found\n");
        }

        fclose(f1);
        fclose(f2);
    }

    return 0;
}
