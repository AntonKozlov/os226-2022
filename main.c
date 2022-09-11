#include <stdio.h>
#include <string.h>

const int MAX_LEN = 1024;
const char *STATEMENT_SEPS = ";";
const char *TOKEN_SEPS = " ";


int lst_retcode = 0;

int echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[]) {
	printf("%d\n", lst_retcode);
	return 0;
}

void clean_line(char* str) {
	while (str && *str != '\0' && *str != '\n')
		str++;
	if (str && *str == '\n')
		*str = '\0';
}

int tokenize(char* statement, char** tokens, int size) {
	char *token, *brkb;
	int cnt = 0;
	for (token = strtok_r(statement, TOKEN_SEPS, &brkb);
		 token && cnt < size;
		 token = strtok_r(NULL, TOKEN_SEPS, &brkb), cnt++) {
		*tokens = token;
		tokens++;
	}
	return cnt;
}

int execute_statement(char* statement) {
	char* tokens[MAX_LEN];
	int argc = tokenize(statement, tokens, MAX_LEN);

	int (*command)(int, char**) = NULL;

	if (argc > 0) {
		if (strcmp("echo", tokens[0]) == 0)
			command = echo;
		if (strcmp("retcode", tokens[0]) == 0)
			command = retcode;
	}

	if (command == NULL)
		return -1;
	return command(argc, tokens);
}

int main(int argc, char *argv[]) {
	char line[MAX_LEN];
	char *statement, *brkt;

	while (fgets(line, MAX_LEN, stdin)) {
		clean_line(line);
		for (statement = strtok_r(line, STATEMENT_SEPS, &brkt);
			 statement;
			 statement = strtok_r(NULL, STATEMENT_SEPS, &brkt)) {
			lst_retcode = execute_statement(statement);
		}
	}
	return 0;
}
