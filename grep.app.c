#include "usyscall.h"

char *strstr(const char *where, const char *what) {
	while (*where != '\0') {
		char* s = where;
		char* n = what;
		while (*s != '\0' && *n != '\0' && *s == *n) {
			s++;
			n++;
		}

		if (*n == '\0') {
			return (char*)where;
		}

		where++;
	}

	return 0;
}

void *memchr(const void *str, int c, long unsigned n) {
	char* end = (char*)(str + n);
	for (char* p = (char*)str; p < end; p++) {
		if (*p == c) {
			return p;
		}
	}

	return 0;
}

void *memmove(void *dst, const void *src, long unsigned n) {
	char buf[n];
	char* d = (char*)dst;
	char* s = (char*)s;
	for (int i = 0; i < n; i++) {
		buf[i] = s[i];
	}
	for (int i = 0; i < n; i++) {
		d[i] = buf[i];
	}

	return d;
}

int main(int argc, char* argv[]) {
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
