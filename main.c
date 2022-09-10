#include <stdio.h>
#include <string.h>
int last_ret_code = 0;

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
	printf("%d\n", last_ret_code);
	return 0;
}

int main(int argc, char *argv[])
{
	char str[1024];
	char **arr1 = (char **)malloc(sizeof(char *) * 128);
	char **arr2 = (char **)malloc(sizeof(char *) * 128);

	while (fgets(str, 1024, stdin) != NULL)
	{
		int ptr1 = 0, ptr2;

		char *tmp = strtok(str, ";\n");
		while (tmp != NULL)
		{
			arr1[ptr1++] = tmp;
			tmp = strtok(NULL, ";\n");
		}

		for (int i = 0; i < ptr1; i++)
		{
			ptr2 = 0;
			arr2[ptr2] = strtok(arr1[i], " ");

			while (arr2[ptr2 - 1] != NULL)
				arr2[++ptr2] = strtok(NULL, " ");
			if (arr2[0] == NULL)
				continue;
			if (strcmp("echo", arr2[0]) == 0)
				last_ret_code = echo(ptr2 - 1, arr2);
			if (strcmp("retcode", arr2[0]) == 0)
				last_ret_code = retcode(ptr2 - 1, arr2);
		}
	}

	free(arr1);
	free(arr2);

	return 0;
}