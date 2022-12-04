#include "usyscall.h"

char *strstr(const char *where, const char *what) {
	for ( ; *where != '\0'; where++) {
		char* char_where = where;
		char* char_what = what;

		for ( ; *char_where != '\0' && *char_what != '\0' && *char_where == *char_what;
			char_where++, char_what++);
		if ('\0' == *char_where) {
			return 0;
		}
		if ('\0' == *char_what) {
			return (char*)where;
		}
	}
	return 0;
}

void *memchr(const void *str, int c, long unsigned n) {
	const char *term = str + n;
	for (const char *pos = str; pos < term; pos++) {
		if (c == *pos) {
			return (void*)pos;
		}
	}
	return (void*)0;
}

void *memmove(void *dst, const void *src, long unsigned n) {
	//https://stackoverflow.com/questions/3572309/memmove-implementation-in-c
	char *char_src = (char*)src;
	char *char_dst = (char*)dst;
	char x[n];
	char *t = x;
	for (unsigned long i = 0; i < n; i++)
		t[i] = char_src[i];
	for (unsigned long i = 0; i < n; i++)
		char_dst[i] = t[i];
	return dst;
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
