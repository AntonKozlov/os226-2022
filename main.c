#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_LENGTH 2048
#define MAX_TOKENS_AMOUNT 128
#define MAX_ARGS_AMOUNT 64
#define MAX_ARG_LENGTH 128

int retcode_count = 0;

int echo(int argc, char* argv[]) 
{
	for (int i = 1; i < argc; ++i) 
	{
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char* argv[]) 
{
}

void fill_tokens(char sourse[], char* tokens[], int* tokens_amount);
void interpret(char* tokens[], int tokens_amount);

int main(int argc, char* argv[]) 
{	
	char input_line[MAX_INPUT_LENGTH];
	while (fgets(input_line, MAX_INPUT_LENGTH, stdin) != NULL)
	{
		char* tokens[MAX_TOKENS_AMOUNT];
		int tokens_amount = 0;
		fill_tokens(input_line, tokens, &tokens_amount);
		
		interpret(tokens, tokens_amount);
	}

	return 0;
}

void fill_tokens(char sourse[], char* tokens[], int* tokens_amount)
{
	char* token_delimiters = ";\n";

	char* token = strtok(sourse, token_delimiters);
	while (token != NULL)
	{
		tokens[(*tokens_amount)++] = token;
		token = strtok(NULL, token_delimiters);
	}
}

void interpret(char* tokens[], int tokens_amount)
{	
	char* args_delimiter = " ";

	for (int i = 0; i < tokens_amount; ++i)
	{
		if (!strncmp(tokens[i], "retcode", 7))
		{
			printf("%d\n", retcode_count);
			retcode_count = 0;
		}
		else if (!strncmp(tokens[i], "echo", 4))
		{
			char* args[MAX_ARGS_AMOUNT];
			for (int i = 0; i < MAX_ARGS_AMOUNT; ++i)
			{
				args[i] = (char*)malloc(MAX_ARG_LENGTH * sizeof(char));
			}
			int args_count = 0;

			char* arg = strtok(tokens[i], args_delimiter);
			while (args_count < MAX_ARGS_AMOUNT && arg != NULL)
			{
				strcpy(args[args_count++], arg);
				arg = strtok(NULL, args_delimiter);
			}

			retcode_count = echo(args_count, args);
		}
	}
}
