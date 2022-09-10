
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STANDARD_BUFFER 128

int echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[]) {
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

int main(int argc, char *argv[]) {
	// IMPL ME
	return 0;
}
