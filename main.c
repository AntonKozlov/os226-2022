#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int INPUT_SIZE = 512;
int COMMAND_BUFFER = 128;
int MAX_ARGC = 64;
int _retcode = 0;

typedef int (*CallBack)(int, char**);
struct command {
    char* name;
    CallBack function;
};

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

void parse(char* input, int *number_of_token, char** parsed_input, char* separator) {
    int cnt = 0;
    parsed_input[cnt++] = strtok(input, separator);
    while (parsed_input[cnt - 1] != NULL) {
        parsed_input[cnt++] = strtok(NULL, separator);
    }
    *number_of_token = cnt - 1;
}

int interprete(char* string, const int number_of_command, const struct command commands_list[]) {
    char** parsed_commands = (char**)malloc(sizeof(char*) * COMMAND_BUFFER);
    int command_number = 0;
    parse(string, &command_number, parsed_commands, ";");
    
    for (int i = 0; i < command_number; i++) {
        char** argv = (char**)malloc(sizeof(char*) * MAX_ARGC);
        int argc = 0;
        parse(parsed_commands[i], &argc, argv, " ");
        
        for (int i = 0; i < number_of_command; i++) {
            if (!strcmp(commands_list[i].name, argv[0])) {
                _retcode = commands_list[i].function(argc, argv);
                break;
            }
        }
        free(argv);
    }
    free(parsed_commands);
    return 1;
}

int main(int argc, char *argv[]) {
    int number_of_command = 2;
    struct command commands_list[] = {
        {"echo", &echo},
        {"retcode", &retcode}
    };


    char input[INPUT_SIZE];
    int argument_number = 0;
    
    while (fgets(input, INPUT_SIZE, stdin) != NULL) {
        char** parsed_input = (char**)malloc(sizeof(char*) * COMMAND_BUFFER);
        parse(input, &argument_number, parsed_input, "\n");
        
        for (int i = 0; i < argument_number; i++) {
            interprete(parsed_input[i], number_of_command, commands_list);
        }
        free(parsed_input);
    }
    return 0;
}
