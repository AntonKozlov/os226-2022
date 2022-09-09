#include <string.h>
#include <stdio.h>

const int MAX_INPUT_SIZE = 1024;
const int MAX_TOKEN_COUNT = 128;
const int MAX_COMMANDS_COUNT = 128;

int _retcode = 0;

int run_command(int tokens_count, char *tokens[MAX_TOKEN_COUNT]);

int echo(int argc, char *argv[]);

int retcode(int argc, char *argv[]);

void run(char *buffer);

int main(int argc, char *argv[]) {
    char buffer[MAX_INPUT_SIZE];
    while (fgets(buffer, MAX_INPUT_SIZE, stdin)) {
        run(buffer);
    }

    return _retcode;
}

int tokenize(char *buffer, char *result[], char delimeters[]) {
    int count = 0;
    char *token = strtok(buffer, delimeters);
    while (token) {
        result[count++] = token;
        token = strtok(NULL, delimeters);
    }
    return count;
}


void run(char *buffer) {
    char *commands[MAX_COMMANDS_COUNT];
    int commands_count = tokenize(buffer, commands, ";\n");
    for (int cmd = 0; cmd < commands_count; cmd++) {
        char *words[MAX_TOKEN_COUNT];
        int words_count = tokenize(commands[cmd], words, " ");
        if (words_count > 0) {
            _retcode = run_command(words_count, words);
        }
    }
}

int run_command(int tokens_count, char *tokens[]) {
    if (strcmp(tokens[0], "echo") == 0) {
        return echo(tokens_count, tokens);
    } else if (strcmp(tokens[0], "retcode") == 0) {
        return retcode(tokens_count, tokens);
    } else {
        return 1;
    }
}

int echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    return argc - 1;
}

int retcode(int argc, char *argv[]) {
    printf("%d\n", _retcode);
    return 0;
}
