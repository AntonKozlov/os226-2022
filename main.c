
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pool.h"

#define STANDARD_INPUT_SIZE 1024
#define STANDARD_BUFFER 256

/// global variable for storing a retcode from functions
int _retcode = 0;

#define APPS_X(X) \
	X(echo)       \
	X(retcode)    \
	X(pooltest)

#define DECLARE(X) static int X(int, char *[]);
APPS_X(DECLARE)
#undef DECLARE

static const struct app {
	const char *name;
	int (*fn)(int, char *[]);
} app_list[] = {
#define ELEM(X) {#X, X},
	APPS_X(ELEM)
#undef ELEM
};

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

static char **parse(char *input, int *output_argument_number) {
	char *command_separators = "\n;";
	char **parsed_input = (char **)malloc(sizeof(char *) * STANDARD_BUFFER);
	int tokens_found = 0;

	parsed_input[tokens_found++] = strtok(input, command_separators);

	while (parsed_input[tokens_found - 1] != NULL) {
		parsed_input[tokens_found++] = strtok(NULL, command_separators);
	}

	*output_argument_number = tokens_found - 1;

	return parsed_input;
}

static int exec(int argc, char **argv) {
	const struct app *app = NULL;
	for (int i = 0; i < ARRAY_SIZE(app_list); ++i) {
		if (!strcmp(argv[0], app_list[i].name)) {
			app = &app_list[i];
			break;
		}
	}

	if (!app) {
		printf("Unknown command\n");
		return 1;
	}

	_retcode = app->fn(argc, argv);
	return _retcode;
}

static int interprete(int argc, char **argv) {
	int arguments_count = 0;
	char *arguments[STANDARD_BUFFER];

	for (int i = 0; i < argc; i++) {
		char *arguments_separators = " ";

		arguments[arguments_count++] = strtok(argv[i], arguments_separators);

		while (arguments[arguments_count - 1] != NULL) {
			arguments[arguments_count++] = strtok(NULL, arguments_separators);
		}

		if (arguments_count > 0) {
			exec(arguments_count - 1, arguments);
		}
		arguments_count = 0;
	}

	return 0;
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
		printf("alloc %ld\n", o ? (o - objmem) : -1);
		return 0;
	} else if (!strcmp(argv[1], "free")) {
		int iobj = atoi(argv[2]);
		printf("free %d\n", iobj);
		pool_free(&objpool, objmem + iobj);
		return 0;
	}
}

int process_input(int argc, char *argv[]) {
	char input[STANDARD_INPUT_SIZE];
	int argumentNumber = 0;
	while (fgets(input, STANDARD_INPUT_SIZE, stdin) != NULL) {
		char **parsed_input = parse(input, &argumentNumber);
		interprete(argumentNumber, parsed_input);
		free(parsed_input);
	}

	return 0;
}

int main(int argc, char *argv[]) {
	return process_input(0, NULL);
}