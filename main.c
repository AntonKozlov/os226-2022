#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 1024
#define RETCODE "retcode"
#define ECHO "echo"

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
    printf("%d\n", argc);
    return 0;
}

void parse(char *input, char **parsed, int *size)
{
    char *p = strtok(input, "; \n");

    while (p != NULL)
    {
        parsed[*size] = p;
        *size = *size + 1;
        p = strtok(NULL, "; \n");
    }
}

void interpretate(char **parsed, int size, int *code)
{
    char *args[MAX_SIZE];
    int i = 0;

    while (i < size)
    {
        int args_size = 0;
        char* p = parsed[i];
        args[args_size++] = p;

        if (strcmp(p, RETCODE) == 0)
            *code = retcode(*code, args);

        if (strcmp(p, ECHO) == 0)
        {
            while (true)
            {
                if (++i >= size)
                    break;
            
                p = parsed[i];

                if (strcmp(p, ECHO) == 0 || strcmp(p, RETCODE) == 0)
                {
                    --i;
                    break;
                }

                args[args_size] = p;
                args_size++;
            }
            *code = echo(args_size, args);
        }
        i++;
    }
}

int main(int argc, char *argv[])
{
    char *input_command[MAX_SIZE];
    int code = 0;

	while (fgets(input_command, MAX_SIZE, stdin) != NULL)
    {
        int input_size = 0;
        char *parsed_commands[MAX_SIZE];

        parse(input_command, parsed_commands, &input_size);
        interpretate(parsed_commands, input_size, &code);
    }

	return 0;
}