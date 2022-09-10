
#include <stdio.h>

int echo(int argc, char *argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

int retcode(int argc, char *argv[]) {
}

int main(int argc, char *argv[]) {
	// IMPL ME
	return 0;
}
