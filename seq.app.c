#include "usyscall.h"
#include "stdlib.h"


int atoi(const char *str) {
	int i = 0;
	for (const char *c = str; *c; c++) {
		i = 10 * i + (*c - '0');
	}
	return i;
}


static void swap_chars(char *a, char *b) {
	char c = *a;
	*a = *b;
	*b = c;
}

int itoa(int v, char *d) {
	const char *const digits = "0123456789";

	char *d_;
	for (d_ = d; v != 0; v /= 10, d_++) {
		*d_ = digits[v % 10];
	}
	size_t len = d_ - d;

	for (size_t i = 0; i < len / 2; i++) swap_chars(d + i, d_ - i - 1);

	return (int) len;
}

int main(int argc, char *argv[]) {
	int n = atoi(argv[1]);
	char buf[32];
	for (int i = 1; i <= n; ++i) {
		int l = itoa(i, buf);
		buf[l] = '\n';
		os_write(1, buf, l + 1);
	}
	os_exit(0);
}
