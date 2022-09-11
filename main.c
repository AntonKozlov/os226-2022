#include <stdio.h>

int echo(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char* argv[]) {
	printf("%d\n", argc);
	return 0;
}

struct Commands {
	void (*echo)(int, char*);

	void (*retcode)(int, char*);
	int retcode_value;

};

int main(int argc, char* argv[]) {

	struct Commands commands = { 
		echo, 
		retcode, 
		default 
	};

	char str[255];
	char* next_token1 = NULL;
	char* next_token2 = NULL;

	while (gets(str))
	{
		char* command = strtok_s(str, ";", &next_token1);

		while (command)
		{

			char* command_argv = strtok_s(command, " ", &next_token2);

			char* argv[255];

			int i = 0;

			for (i; command_argv; i++)
			{
				argv[i] = command_argv;
				command_argv = strtok_s(NULL, " ", &next_token2);
				
			}

			if (strcmp(argv[0], "echo") == 0) {
				commands.retcode_value = i-1;
				commands.echo(i, argv);
			}
			else if (strcmp(argv[0], "retcode") == 0) {
				commands.retcode(commands.retcode_value, argv);
			}
			else if () {
			}

			command = strtok_s(NULL, ";", &next_token1);
		}
	}
	return 0;
}
