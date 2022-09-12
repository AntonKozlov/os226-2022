#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 1024

int retCode = 0;

int echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    return argc - 1;
}

int retcode(int argc, char *argv[]) {
    printf("%d\n", retCode);
    return 0;
}


int main(int argc, char *argv[]) {

    char *input = (char *) malloc(sizeof(char *) * MAX_LINE_SIZE);
    char **args = (char **) malloc(sizeof(char *) * MAX_LINE_SIZE);
    char **commands = (char**)malloc(sizeof(char*) * MAX_LINE_SIZE);
    char* command;
    char sep[10] = ";\n";
    int count;
    int exit = 0;

    while (exit != 2) {
        fgets(input, MAX_LINE_SIZE, stdin);
        if (input[0] == '\n') {
            exit++;
        }

        int countArgs;
        count = 0;
        command = strtok(input, sep);

        while (command != NULL) {
            commands[count++] = command;
            command = strtok(NULL, sep);
        }

        for (int i = 0; i < count; i++) {
            countArgs = 0;
            args[countArgs] = strtok(commands[i], " ");

            while (args[countArgs - 1] != NULL) {
                countArgs++;
                args[countArgs] = strtok(NULL, " ");
            }

            if (args[0] == NULL) {
                continue;
            }
            if (strcmp("echo", args[0]) == 0) {
                retCode = echo(countArgs - 1, args);
            }
            if (strcmp("retcode", args[0]) == 0) {
                retCode = retcode(countArgs - 1, args);
            }
        }
    }

    free(input);
    free(commands);
    free(command);
    free(args);

    return 0;
}