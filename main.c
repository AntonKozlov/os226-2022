
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define TOTAL_AVAILABLE_COMMANDS 2

#define COMMAND_NOT_FOUND 127

int echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[]) {
	return 0;
}

char *read_input(void) {
	char *buffer = malloc(BUFFER_SIZE);
	char *input = fgets(buffer, BUFFER_SIZE, stdin);
	int input_size = 0;

	while (buffer && strlen(buffer) >= BUFFER_SIZE - 1 && buffer[BUFFER_SIZE - 1] != '\n') {  
		input = realloc(input, input_size + BUFFER_SIZE);
		input_size += BUFFER_SIZE;

		if (input == NULL) {
			fprintf(stderr, "Failed to reallocate memory to read the inputs\n");
		}

		strcat(input, buffer);
		buffer = fgets(buffer, BUFFER_SIZE, stdin);
	}

	if (input)
		input[strcspn(input, "\n")] = 0;

	return input;
}

char **flush_buffer(char **destination, int destination_size, char **buffer, int buffer_size) {
	destination = realloc(destination, sizeof(char*) * (destination_size + buffer_size));
	
	if (destination == NULL) {
		fprintf(stderr, "Failed to reallocate memory to parse commands\n");
		return NULL;
	}

	memcpy(destination + destination_size, buffer, sizeof(char*) * buffer_size);

	return destination;
}

// description: split `input` into non-empty strings by the `delimiter`
// and put the results in `destination`
// return value: number of parsed tokens
int parse(char ***result, char *input, char *delimiters) {
	char *buffer[BUFFER_SIZE];
	int buffer_index = 0;
	
	char **result_list = NULL;
	int result_list_size = 0;

	for (char *current_token = strtok(input, delimiters); current_token; current_token = strtok(NULL, delimiters)) {
		if (buffer_index >= BUFFER_SIZE) {
			result_list = flush_buffer(result_list, result_list_size, buffer, BUFFER_SIZE);
			result_list_size += BUFFER_SIZE;
			
			if (result_list == NULL) {
				fprintf(stderr, "Failed to flush buffer\n");
			}
		}
		
		buffer[buffer_index] = current_token;
		buffer_index++; 
	}

	if (buffer_index > 0) {
		result_list = flush_buffer(result_list, result_list_size, buffer, buffer_index);
		result_list_size += buffer_index;
	}

	*result = result_list;
	return result_list_size;
}

typedef struct command {
	char *name;
	int (*function_ptr)(int, char**);
} shell_command;

int execute_command(shell_command *available_commands, int argc, char *argv[]) {
	static int return_code = 0;

	if (!available_commands) {
		return return_code = COMMAND_NOT_FOUND;
	}

	if (!strcmp("retcode", argv[0])) {
		printf("%d\n", return_code);
		return return_code = 0;
	}

	for (int i = 0; i < TOTAL_AVAILABLE_COMMANDS; i++) {
		if (!strcmp(available_commands[i].name, argv[0])) {
			return return_code = available_commands[i].function_ptr(argc, argv);
		}
	}
	return return_code = COMMAND_NOT_FOUND;	
}

int main(int argc, char *argv[]) {
	shell_command echo_command = {"echo", echo};
	shell_command retcode_command = {"retcode", echo};
	shell_command command_list[2] = {echo_command, retcode_command};

	char *user_input = NULL;
	while ((user_input = read_input()) != NULL) {
		char **parsed_instructions = NULL;
		int total_instructions = parse(&parsed_instructions, user_input, ";");

		if (!parsed_instructions) {
			continue;
		}

		for (int i = 0; i < total_instructions; i++) {
			char **parsed_arguments = NULL;
			int arguments_count = parse(&parsed_arguments, parsed_instructions[i], " ");

			if (!parsed_arguments) {
				fprintf(stderr, "Failed to parse arguments\n");
			}

			execute_command(command_list, arguments_count, parsed_arguments);	
		}
	}
	return 0; 
}
