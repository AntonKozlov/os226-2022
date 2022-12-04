#include "usyscall.h"


int atoi(const char *str) {
	int j = 0;
	while ('\0' != *str) {
		j = j * 10 + *str++ - '0';
	}
	return j;
}

int itoa(int value, char *d) {
	char tmp[16];
	char *tp = tmp;
	int i;
	unsigned int v = (unsigned int)value;

	int sign = (value < 0);

	if (sign) {
		v = -value;
	}

	while (v || tp == tmp) {
		i = v % 10;
		v /= 10;
		*tp++ = i + '0';
	}
	int len = tp - tmp;
	if (sign) {
		*d++ = '-';
		len++;
	}
	while (tp > tmp) {
		*d++ = *--tp;
	}
	return len;
}

int main(int argc, char* argv[]) {
	int n = atoi(argv[1]);
	char buf[32];
	for (int i = 1; i <= n; ++i) {
		int l = itoa(i, buf);
		buf[l] = '\n';
		os_write(1, buf, l + 1);
	}
	os_exit(0);
}
