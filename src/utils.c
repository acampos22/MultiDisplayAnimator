#include "../includes/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 128

char **read_script(const char *filename, int *line_count) {
    FILE *file = fopen(filename, "r");
    if (!file) return NULL;

    char **lines = malloc(MAX_LINES * sizeof(char *));
    if (!lines) return NULL;

    char buffer[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(buffer, sizeof(buffer), file) && count < MAX_LINES) {
        buffer[strcspn(buffer, "\n")] = '\0';  // quitar newline
        lines[count] = strdup(buffer);
        count++;
    }

    fclose(file);
    *line_count = count;
    return lines;
}
