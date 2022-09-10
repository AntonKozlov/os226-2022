#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//------CONST------//
#define MAX_INPUT_SIZE 1024
#define MAX_INPUT_ARGUMENTS 512
//-----------------//

//------GLOBAL_VARIABLES------//
int RETCODE = 0;
//----------------------------//

//------PAYLOAD------//
int echo (int argc, char *argv[]);
int retcode(int argc, char *argv[]);

typedef void (*FunctionCallback)(int, char**);
struct commands
{
	char* name;
	FunctionCallback function;
} commands_list[] = {
	{"echo",    (FunctionCallback) &echo},
        {"retcode", (FunctionCallback) &retcode}
};
//-------------------//

int echo(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	
	RETCODE = argc - 1;
	return RETCODE;
}

int retcode(int argc, char *argv[])
{
	printf("%d\n", RETCODE);
	RETCODE = 0;
	return RETCODE;
}

// if is_parse_on_commands then parse with only ';', else -- with ';' , '\n' and ' '.
void parse(char* input, int* parsed_input_arguments_counter, char** parsed_input_array, bool is_parse_on_commands)
{
	char* input_delimit = "; \n";
	if (is_parse_on_commands)
	{
		input_delimit = ";;;";
	}
	
	char* parsed_input_word = strtok(input, input_delimit);

	while (NULL != parsed_input_word)
	{
		parsed_input_array[(*parsed_input_arguments_counter)++] = parsed_input_word;
		parsed_input_word = strtok(NULL, input_delimit);
	}
}

void interprete(char* command)
{
	char* array_of_parsed_command[MAX_INPUT_ARGUMENTS];
	int arguments_counter = 0;
	bool isThereNoCommand = false;

	parse(command, &arguments_counter, array_of_parsed_command, false);
	for (int i = 0; i < sizeof commands_list/sizeof (struct commands); i++)
	{
		if (!strcmp(array_of_parsed_command[0], commands_list[i].name))
		{
			commands_list[i].function(arguments_counter, array_of_parsed_command);
			break;
		}
		else
		{
			isThereNoCommand = true;
		}
	}
	if (isThereNoCommand)
	{
		RETCODE = 1;
	}
}

int main(int argc, char *argv[])
{
	char input[MAX_INPUT_SIZE];

	while (NULL != fgets(input, MAX_INPUT_SIZE, stdin))
	{
		char* array_of_commands_from_input[MAX_INPUT_ARGUMENTS]; // Массив команд (здесь будут команды, которые получим, распарсив строку с помощью ';').
		int commands_counter = 0;
		parse(input, &commands_counter, array_of_commands_from_input, true);
		
		for (int i = 0; i < commands_counter; i++)
		{
			interprete(array_of_commands_from_input[i]);
		}
	}
	return 0;
}
