#include <stdio.h>
#include <string.h>

int last_value = 0;

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
	printf("%d\n", last_value);
	return 0;
}

void processing(char* words)
{
	char* semicolon[64];
	char* space[64];

	//Этот код разбивает полученную строку по точке с запятой
	semicolon[0] = strtok(words, ";");

	int i = 1;
	while (semicolon[i - 1] != NULL)
	{
		semicolon[i] = strtok(NULL, ";");
		i++;
	}

	//Этот код разбивает полученные строки по пробелу и обрабатывает их
	for (int j = 0; j < i; j++)
	{
		space[0] = strtok(semicolon[j], " ");
		int k = 1;

		while (space[k - 1] != NULL)
		{
			space[k] = strtok(NULL, " ");
			k++;
		}

		if (space[0] == NULL)
		{
			continue;
		}

		if (strcmp(space[0], "echo") == 0)
		{
			echo(k - 1, space);
			last_value = k - 2;
		}

		if (strcmp(space[0], "retcode") == 0)
		{
			printf("%d\n", last_value);
		}
	}

	return;
}

int main(int argc, char* argv[])
{
	char words[64];

	while (gets(words) != NULL) 
	{ 
		processing(words);
	}

	return 0;
}