#include <stdio.h>
#include <stting.h>

#define LIST_OF_COMMANDS \
    X(echo) \
    X(retcode)


int echo(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int RETCODE_VALUE = 0;

int retcode(int argc, char* argv[]) {
	printf("%d\n", RETCODE_VALUE);
	return 0;
}


struct commands
{
	char* name;
	int (*function)(int, char*);
} commands_list[] = {
#define X(name) {#name, name},
LIST_OF_COMMANDS
#undef X
};

int main(int argc, char* argv[]) {

	char str[255];
	char* next_token1 = NULL;
	char* next_token2 = NULL;

	while (gets(str))
	{
		char* command = strtok_r(str, ";", &next_token1);

		while (command)
		{

			char* command_argv = strtok_r(command, " ", &next_token2);

			char* argv[255];

			int i = 0;

			for (i; command_argv; i++)
			{
				argv[i] = command_argv;
				command_argv = strtok_r(NULL, " ", &next_token2);
			}

			for (int j = 0; j < sizeof commands_list / sizeof(struct commands); j++)
			{
				if (!strcmp(argv[0], commands_list[j].name))
				{
					RETCODE_VALUE = commands_list[j].function(i, argv);
					break;
				}
			}
			command = strtok_r(NULL, ";", &next_token1);
		}
	}
	return 0;
}
