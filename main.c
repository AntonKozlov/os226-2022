#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_LENGTH 512
#define MAX_NUMBER_OF_ARG 32


int _retcode = 0;

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

int main(int argc, char *argv[]) {

	char input[MAX_INPUT_LENGTH];
	char *token;
	char *arg;
	char *args[MAX_NUMBER_OF_ARG];
	int args_counter = 0;
	char *token_ptr;
	char *arg_ptr;

	while (fgets(input, MAX_INPUT_LENGTH, stdin) != NULL) {		
		token = strtok_r(input, ";\n", &token_ptr);

		while (token != NULL) {
			arg = strtok_r(token, " ", &arg_ptr);

			while (arg != NULL) {
				args[args_counter] = arg;
				arg = strtok_r(NULL, " ", &arg_ptr);
				args_counter++;
			}

			if (!strncmp(args[0], "retcode", 7)) {
				_retcode = retcode(args_counter, args);
			}
			else if (!strncmp(args[0], "echo", 4)) {
				_retcode = echo(args_counter, args);
			}

			args_counter = 0;
			token = strtok_r(NULL, ";\n", &token_ptr);
		}
	}

	return 0;
}
