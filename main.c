﻿
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pool.h"

static int g_retcode;

#define APPS_X(X) \
        X(echo) \
        X(retcode) \
        X(pooltest) \


#define DECLARE(X) static int X(int, char *[]);
APPS_X(DECLARE)
#undef DECLARE

static const struct app {
	const char* name;
	int (*fn)(int, char* []);
} app_list[] = {
#define ELEM(X) { # X, X },
		APPS_X(ELEM)
#undef ELEM
};

static int echo(int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
	}
	return argc - 1;
}

static int retcode(int argc, char* argv[]) {
	printf("%d\n", g_retcode);
	return 0;
}

static int exec(int argc, char* argv[]) {
	const struct app* app = NULL;
	for (int i = 0; i < ARRAY_SIZE(app_list); ++i) {
		if (!strcmp(argv[0], app_list[i].name)) {
			app = &app_list[i];
			break;
		}
	}

	if (!app) {
		printf("Unknown command\n");
		return 1;
	}

	g_retcode = app->fn(argc, argv);
	return g_retcode;
}

static int pooltest(int argc, char* argv[]) {
	struct obj {
		void* field1;
		void* field2;
	};
	static struct obj objmem[4];
	static struct pool objpool = POOL_INITIALIZER_ARRAY(objmem);

	if (!strcmp(argv[1], "alloc")) {
		struct obj* o = pool_alloc(&objpool);
		printf("alloc %d\n", o ? (o - objmem) : -1);
		return 0;
	}
	else if (!strcmp(argv[1], "free")) {
		int iobj = atoi(argv[2]);
		printf("free %d\n", iobj);
		pool_free(&objpool, objmem + iobj);
		return 0;
	}
}

int shell(int argc, char* argv[]) {
	char line[256];
	while (fgets(line, sizeof(line), stdin)) {
		const char* comsep = "\n;";
		char* stcmd;
		char* cmd = strtok_r(line, comsep, &stcmd);
		while (cmd) {
			const char* argsep = " ";
			char* starg;
			char* arg = strtok_s(cmd, argsep, &starg);
			char* argv[256];
			int argc = 0;
			while (arg) {
				argv[argc++] = arg;
				arg = strtok_r(NULL, argsep, &starg);
			}
			argv[argc] = NULL;

			if (!argc) {
				break;
			}

			exec(argc, argv);

			cmd = strtok_r(NULL, comsep, &stcmd);
		}
	}
	return 0;
}


int main(int argc, char* argv[]) {
	shell(0, NULL);

	return 0;
}
