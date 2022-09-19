#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pool.h"

static int g_retcode;

#define APPS_X(X) \
        X(echo) \
        X(retcode) \
        X(pooltest) \


#define DECLARE(X) static int X(int, char *[]);

APPS_X(DECLARE)

#undef DECLARE

static const struct app {
    const char *name;

    int (*fn)(int, char *[]);
} app_list[] = {
#define ELEM(X) { # X, X },
        APPS_X(ELEM)
#undef ELEM
};

const int MAX_INPUT_SIZE = 1024;
const int MAX_TOKEN_COUNT = 128;
const int MAX_COMMANDS_COUNT = 128;

static int _retcode = 0;

static void run(char *buffer);

static int tokenize(char *buffer, char *result[], char delimeters[]);

static int run_command(int tokens_count, char *tokens[MAX_TOKEN_COUNT]);

static int echo(int argc, char *argv[]);

static int retcode(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    char buffer[MAX_INPUT_SIZE];
    while (fgets(buffer, MAX_INPUT_SIZE, stdin)) {
        run(buffer);
    }

    return _retcode;
}

static int pooltest(int argc, char *argv[]) {
    struct obj {
        void *field1;
        void *field2;
    };
    static struct obj objmem[4];
    static struct pool objpool = POOL_INITIALIZER_ARRAY(objmem);

    if (!strcmp(argv[1], "alloc")) {
        struct obj *o = pool_alloc(&objpool);
        printf("alloc %d\n", o ? (o - objmem) : -1);
        return 0;
    } else if (!strcmp(argv[1], "free")) {
        int iobj = atoi(argv[2]);
        printf("free %d\n", iobj);
        pool_free(&objpool, objmem + iobj);
        return 0;
    }
}

static void run(char *buffer) {
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

static int tokenize(char *buffer, char *result[], char delimeters[]) {
    int count = 0;
    char *token = strtok(buffer, delimeters);
    while (token) {
        result[count++] = token;
        token = strtok(NULL, delimeters);
    }
    return count;
}

static int run_command(int tokens_count, char *tokens[]) {
    for (int i = 0; i < sizeof(app_list) / sizeof(struct app); ++i) {
        if (!strcmp(app_list[i].name, tokens[0])) {
            return app_list[i].fn(tokens_count, tokens);
        }
    }
    return 1;
}

static int echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    return argc - 1;
}

static int retcode(int argc, char *argv[]) {
    printf("%d\n", _retcode);
    return 0;
}
