
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 512
#define MAX_ARGUMENT_SIZE 128
#define MAX_ARGUMENT_COUNT 32


const char CONST_RETCODE_KEYWORD[] = "retcode";
const char CONST_ECHO_KEYWORD[] = "echo";
const char CONST_PARSE_DELIMETERS[] = " ;\n";
int LASTRETCODE = 0;

int echo(int argc, char *argv[]) 
{
	for (int i = 1; i < argc; ++i)
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	return argc - 1;
}

int retcode() 
{
	printf("%d\n", LASTRETCODE);
	return 0;
}

void parse(char* InputString)
{
	char* ProcessedPart = strtok(InputString, CONST_PARSE_DELIMETERS);
	int Elements = 0;
	char SplitInput[MAX_ARGUMENT_COUNT][MAX_ARGUMENT_SIZE];
	while (ProcessedPart) 
	{
		strcpy(SplitInput[Elements++], ProcessedPart);
		ProcessedPart = strtok(NULL, CONST_PARSE_DELIMETERS);
	}

	for (int i = 0; i < Elements; i++)
	{
		if (!strcmp(SplitInput[i], CONST_RETCODE_KEYWORD))
			LASTRETCODE = retcode();
		if (!strcmp(SplitInput[i], CONST_ECHO_KEYWORD))
		{
			int argc = 1;
			char *argv[MAX_ARGUMENT_COUNT];
			for (int alloc = 1; alloc < MAX_ARGUMENT_COUNT; alloc++)
				argv[alloc] = (char*)malloc(MAX_ARGUMENT_SIZE);

			for (int j = i + 1; j < Elements; j++)
			{
				if (!strcmp(SplitInput[j], CONST_ECHO_KEYWORD) || !strcmp(SplitInput[j], CONST_RETCODE_KEYWORD))
					break;
				else 
					strcpy(argv[argc++], SplitInput[j]);
			}
			LASTRETCODE = echo(argc, argv);
			
			for (int alloc = 1; alloc < MAX_ARGUMENT_COUNT; alloc++)
				free(argv[alloc]);
		}
	}
	free(ProcessedPart);
}

int main(int argc, char *argv[]) 
{
	char InputString[MAX_INPUT_SIZE];
	while (fgets(InputString, MAX_INPUT_SIZE, stdin))
	{
		parse(InputString);
	}
	return 0;
}
