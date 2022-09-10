
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STANDARD_INPUT_SIZE 1024
#define STANDARD_BUFFER 128

/// global variable for storing a retcode from functions
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

char** parse(char* input, int *output_argument_number) {
	char* command_separators = " \n;";
	char** parsed_input = (char**)malloc(sizeof(char*) * STANDARD_BUFFER);
	int tokens_found = 0;

	parsed_input[tokens_found++] = strtok(input, command_separators);

	while (parsed_input[tokens_found - 1] != NULL) {
		parsed_input[tokens_found++] = strtok(NULL, command_separators);;
	}

	*output_argument_number = tokens_found - 1;

	return parsed_input;
}

void interprete(int argc, char **argv) {
	int arguments_count = 0;
	char* arguments[STANDARD_INPUT_SIZE];

	for (int counter = 0; counter < argc; counter++)
	{
		char* token = argv[counter];

		if (!strcmp(token, "retcode")) {
			_retcode = retcode(arguments_count, arguments);
		}
		else if (!strcmp(token, "echo")) {
			arguments_count = 0;
			arguments[arguments_count++] = token;

			while ((counter + 1 != argc) && ((strcmp(argv[++counter], "retcode")) && (strcmp (argv[counter], "echo"))))
			{
				arguments[arguments_count++] = argv[counter];
			}

			_retcode = echo(arguments_count, arguments);
			counter--;
		}
		else {
			break;
		}
	}

	return;
}

int main(int argc, char *argv[]) {
	
	char input[STANDARD_INPUT_SIZE];
	int argumentNumber = 0;
	while (fgets(input, STANDARD_INPUT_SIZE, stdin) != NULL)
	{
		char** parsed_input = parse(input, &argumentNumber);
		interprete(argumentNumber, parsed_input);
		free(parsed_input);
	}

	return 0;
}
