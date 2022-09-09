#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_LENGTH 256
#define MAX_ARG_NUMBER 21

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
    return 0;
}

int main(int argc, char *argv[])
{
	char line[MAX_LINE_LENGTH];
    char* token;
    char* command;
    char* arg;
    char line_copy[MAX_LINE_LENGTH];
    char* argv_[MAX_ARG_NUMBER];
    for(int i = 0; i < MAX_ARG_NUMBER; i++)
    {
        argv_[i] = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
    }

    char* pointer = fgets(line, MAX_LINE_LENGTH, stdin);
    int last_code = 0;
    int count_args = 1;
    int offset = 0;
    while (pointer != NULL)
    {
        strcpy(line_copy, line);
        token = strtok(line, ";\n");
        while (token != NULL) 
        {
            offset = strlen(token);
            command = strtok(token, " ");
            if (strncmp(command, "retcode", 7) == 0)
            {
                printf("%d\n", last_code);
                last_code = 0;
            }
            else if (strncmp(command, "echo", 4) == 0)
            {
                arg = strtok(NULL, " ");
                for(int i = 1; i < MAX_ARG_NUMBER && arg != NULL; i++)
                {
                    strcpy(argv_[i], arg);
                    arg = strtok(NULL, " ");
                    count_args++;
                }

                last_code = echo(count_args, argv_);
                count_args = 1;
            }

            strcpy(line, line_copy);
            token = strtok(token + offset, ";\n");
        }

        pointer = fgets(line, MAX_LINE_LENGTH, stdin);
        offset = 0;
    }

    for(int i = 0; i < MAX_ARG_NUMBER; i++)
    {
        free(argv_[i]);
    }

	return 0;
}

