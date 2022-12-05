#include <stddef.h>
#include "usyscall.h"

char *strstr(const char *where, const char *what) {
	for (int i = 0; where[i]; i++) {
		int j = 0;
		while (where[i + j] && what[j] && where[i + j] == what[j]) ++j;
		if (!(where[i + j] && what[j])) return (char *) (where + i);
	}
	return NULL;
}

void *memchr(const void *str, int c, long unsigned n) {
	for (const char *b = str, *e = str + n; b < e; ++b) {
		if (*b == c) return (void *) b;
	}
	return NULL;
}

void *memmove(void *dst, const void *src, long unsigned n) {
	char *dst_c = dst;
	const char *src_c = src;
	for (size_t i = 0; i < n; i++) dst_c[i] = src_c[i];
	return dst;
}

int main(int argc, char *argv[]) {
	char buf[5];
	int len;
	int o = 0;
	while (0 < (len = os_read(0, buf + o, sizeof(buf) - o))) {
		len += o;
		char *p = buf;
		char *p2 = memchr(p, '\n', len);
		while (p2) {
			*p2 = '\0';
			if (strstr(p, argv[1])) {
				os_write(1, p, p2 - p);
				os_write(1, "\n", 1);
			}
			p = p2 + 1;
			p2 = memchr(p, '\n', len - (p - buf));
		}
		o = len - (p - buf);
		memmove(buf, p, o);
	}

	os_exit(0);
}
