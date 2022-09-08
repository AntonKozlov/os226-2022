#define _CRT_SECURE_NO_WARNINGS
#define SIZE_OF_INPUT 80
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//----------global----------
int _ret_code = 0;
//--------------------------

int echo(int argc, char* argv[]);
int retcode(int argc, char* argv[]);

void interpret(char** parsed_input, int size_of_parsed_input)
{
    char* args[SIZE_OF_INPUT];
    int size_of_args = 0;

    for (int i = 0; i < size_of_parsed_input; ++i)
    {
        char* token = parsed_input[i];
        args[size_of_args++] = token;

        if (!strcmp(token, "retcode"))
        {
            retcode(size_of_args, args);
        }
        else if (!strcmp(token, "echo"))
        {
            while (1)
            {
                if (i + 1 >= size_of_parsed_input)
                {
                    break;
                }

                token = parsed_input[++i];

                if (!strcmp(token, "echo") || !strcmp(token, "retcode"))
                {
                    i--;
                    break;
                }

                args[size_of_args++] = token;
            }

            echo(size_of_args, args);
        }

        size_of_args = 0;
    }
}

int echo(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    _ret_code = argc - 1;

    return _ret_code;
}

int retcode(int argc, char* argv[])
{
    printf("%d\n", _ret_code);

    return 0;
}

int main(int argc, char* argv[])
{
    char* input[SIZE_OF_INPUT];
    char* sep = " ;\n";

    while (fgets(input, SIZE_OF_INPUT, stdin) != NULL)
    {
        char* parsed_input[80];
        int size_of_parsed_input = 0;

        //printf("%s\n", input);

        char* token = strtok(input, sep);

        while (token!= NULL)
        {
            parsed_input[size_of_parsed_input++] = token;
            token = strtok(NULL, sep);
        }

        interpret(parsed_input, size_of_parsed_input);
    }

    return 0;
}