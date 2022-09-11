
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 256
#define ARGS_BUFFER_SIZE 256

int last_retcode = 0;

int echo(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[])
{
	printf("%d\n", last_retcode);
	return 0;
}

void process(char *input)
{
	char *cmd_divs = ";\n";
	char *args_divs = " \n";

	// Pointer is used by strtok_r to store its internal state
	char *first_ptr;
	char *second_ptr;

	char *args_str = strtok_r(input, cmd_divs, &first_ptr);

	// Dividing the commands and processing
	while (args_str != NULL)
	{
		char *arg = strtok_r(args_str, args_divs, &second_ptr);

		int args_count = 0;
		char *args[ARGS_BUFFER_SIZE];

		// Filling in args array
		while (arg != NULL)
		{
			args[args_count] = arg;
			args_count++;

			arg = strtok_r(NULL, args_divs, &second_ptr);
		}

		// Switching and executing the command
		if (strcmp(args[0], "echo") == 0)
		{
			last_retcode = echo(args_count, args);
		}
		else if (strcmp(args[0], "retcode") == 0)
		{
			last_retcode = retcode(args_count, args);
		}

		args_str = strtok_r(NULL, cmd_divs, &first_ptr);
	}
}

int main(int argc, char *argv[])
{
	char input[INPUT_BUFFER_SIZE];

	while (fgets(input, sizeof(input), stdin) != NULL)
	{
		process(input);
	}

	return 0;
}
